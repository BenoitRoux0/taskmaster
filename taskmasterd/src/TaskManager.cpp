#include "TaskManager.hpp"

#include <csignal>
#include <print>
#include <ranges>

#include "Logger.hpp"
#include "serializer.hpp"
#include "SignalSocket.hpp"
#include <sys/wait.h>

void TaskManager::loadConf(const std::optional<std::string>& confFile) {
	try {
		if (confFile.has_value())
			_conf = confFile.value();
		auto confParsed = toml::parse(_conf, toml::spec::v(1, 1, 0));

		_tasksConfs = toml::find<std::map<std::string, TaskConf>>(confParsed, "programs");
	} catch (toml::syntax_error& e) {
		std::print("{}", e.what());
	}
}

void TaskManager::stop() {
	_server.stop();
}

void TaskManager::handleDeath(pid_t pid, int32_t status) {
	const auto end = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
	for (auto& [name, task]: _runningTasks) {
		if (task._pid == pid) {
			_logger.write("{} is dead", name._name);
			task.status = expected;
			task.end = end;
			task.procStatus = status;

			if (_tasksConfs[name._name].start_time.has_value()) {
				auto                          time = _tasksConfs[name._name].start_time.value();
				std::chrono::duration<double> diff = end - task.getStart();

				if (diff < std::chrono::seconds(time))
					task.status = unexpected;
			}

			if (WIFEXITED(status)) {
				const auto exit_codes = _tasksConfs[name._name].getExitCodes();

				if (std::ranges::find(exit_codes, WEXITSTATUS(status)) == exit_codes.end()) {
					task.status = unexpected;
				}
			}

			if (WIFSIGNALED(status)) {
				const auto exp_sig = _tasksConfs[name._name].getStopSig();

				if (WTERMSIG(status) != exp_sig) {
					task.status = unexpected;
				}
			}

			task._pid = -1;
			std::erase_if(_stoppingTasks, [name](const auto& current) {
				return current == name;
			});
		}
	}
}

void TaskManager::run() {
	_server.bind(12345);

	if (!_server.isReady())
		return;

	_server.onHttpRequest([&](const auto& request) { return this->_onHttpRequest(request); });

	_server.onChildRequest([&](const auto& siginfo) { this->_onChildRequest(siginfo); });

	_server.onWakeUp([&](auto delta) { this->_onWakeUp(delta); });

	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGCHLD);
	sigaddset(&set, SIGHUP);
	sigaddset(&set, SIGTERM);
	sigaddset(&set, SIGINT);
	pthread_sigmask(SIG_BLOCK, &set, nullptr);

	_server.registerSocket(std::make_shared<SignalSocket>(_server, -1, &set));
	startPrograms();
	_server.run();
}

void TaskManager::startPrograms() {
	for (const auto& [name,task]: _tasksConfs) {
		int pid = fork();
		if (pid == 0) {
			_server.clearConnections();
			execle("/bin/bash", "bash", "-c", task.cmd.c_str(), nullptr, environ);
		} else {
			auto id = RunningTaskId(name, 0);
			_runningTasks[id] = RunningTask(pid);
			_logger.write("Launching program: {}", name);
		}
	}
}

HttpResponse TaskManager::_onHttpRequest(const HttpRequest& request) {
	_logger.write("received: {}", request.getRawUrl());

	if (request.getMethod() == "GET") {
		if (request.getRawUrl() == "/tasks") {
			std::vector<RunningTask> data{};

			for (const auto& task: _runningTasks | std::views::values) {
				data.push_back(task);
			}

			return {stackixx::serialize(data)};
		}
		if (*request.getUrl().begin() == "task") {
			return this->_getTaskDetails(request);
		}
	}

	if (request.getMethod() == "POST") {
		if (request.getUrl().size() == 3 && request.getUrl()[0] == "task" && request.getUrl()[2] == "stop") {
			return this->_stopTask(request);
		}
	}

	return {"404", ""};
}

void TaskManager::_onChildRequest(const signalfd_siginfo& siginfo) {
	int pid = 1;
	int status;

	switch (siginfo.ssi_signo) {
		case SIGCHLD:
			_logger.write("A child is dead");
			while (pid > 0) {
				pid = waitpid(-1, &status, WNOHANG);

				if (pid > 0)
					this->handleDeath(pid, status);
			}
			break;
		case SIGHUP:
			this->loadConf(std::nullopt);
			break;
		case SIGTERM:
		case SIGINT:
			this->stop();
			break;
	}
}

void TaskManager::_onWakeUp(std::chrono::milliseconds delta) {
	for (auto taskId: _stoppingTasks) {
		if (_runningTasks[taskId].decreaseStopTime(delta)) {
			kill(_runningTasks[taskId]._pid, SIGKILL);
		}
	}
}

TaskManager::TaskManager() = default;

TaskManager::~TaskManager() = default;


HttpResponse TaskManager::_getTaskDetails(const HttpRequest& request) {
	if (request.getUrl().size() == 2) {
		auto                     name = request.getUrl()[1];
		std::vector<RunningTask> tasks{};

		for (const auto& [id, task]: _runningTasks) {
			if (id._name == name)
				tasks.push_back(task);
		}

		return {stackixx::serialize(tasks)};
	}

	if (request.getUrl().size() == 3) {
		auto name = request.getUrl()[1];
		auto index = std::stoi(request.getUrl()[2]);

		std::vector<RunningTask> tasks{};

		for (const auto& [id, task]: _runningTasks) {
			if (id._name == name && id._index == index)
				tasks.push_back(task);
		}

		return {stackixx::serialize(tasks)};
	}

	return {"400", ""};
}

HttpResponse TaskManager::_stopTask(const HttpRequest& request) {
	if (request.getUrl().size() != 3) {
		return {"400", ""};
	}

	auto name = request.getUrl()[1];

	for (auto& [id, task]: _runningTasks) {
		if (id._name == name && (task.status == running || task.status == starting)) {
			auto sig = _tasksConfs[id._name].getStopSig();
			task.setStopTime(_tasksConfs[id._name].getStopTime());
			_stoppingTasks.insert(id);
			kill(task._pid, sig);
		}
	}

	return {""};
}
