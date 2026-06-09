#include "TaskManager.hpp"

#include <csignal>
#include <print>
#include <ranges>

#include "Logger.hpp"
#include "serializer.hpp"
#include "SignalSocket.hpp"
#include <sys/wait.h>

TaskManager TaskManager::_instance{};

void TaskManager::loadConf(const std::optional<std::string>& confFile) {
	try {
		if (confFile.has_value())
			_conf = confFile.value();
		auto confParsed = toml::parse(_conf, toml::spec::v(1, 1, 0));

		auto serverConf = toml::find_or<ServerConf>(confParsed, "server", {.port = 3060});
		tasksConfs = toml::find<std::map<std::string, TaskConf>>(confParsed, "programs");

		confHttpServer(serverConf);
	} catch (toml::syntax_error& e) {
		std::print("{}", e.what());
	}
}

void TaskManager::stop() {
	server.stop();
}

void TaskManager::handleDeath(pid_t pid, int32_t status) {
	const auto end = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
	auto       logger = Logger::getInstance("Task master", stdout);

	for (auto& [name, task]: runningTasks) {
		if (task._pid == pid) {
			logger->write("{} is dead", name._name);
			task.status = expected;
			task.end = end;
			task.procStatus = status;

			if (tasksConfs[name._name].start_time.has_value()) {
				auto                          time = tasksConfs[name._name].start_time.value();
				std::chrono::duration<double> diff = end - task.getStart();

				if (diff < std::chrono::seconds(time))
					task.status = unexpected;
			}

			if (WIFEXITED(status)) {
				const auto exit_codes = tasksConfs[name._name].getExitCodes();

				if (std::ranges::find(exit_codes, WEXITSTATUS(status)) == exit_codes.end()) {
					task.status = unexpected;
				}
			}

			if (WIFSIGNALED(status)) {
				const auto exp_sig = tasksConfs[name._name].getStopSig();

				if (WTERMSIG(status) != exp_sig) {
					task.status = unexpected;
				}
			}

			task._pid = -1;
			std::erase_if(stoppingTasks, [name](const auto& current) {
				return current == name;
			});
		}
	}
}

void TaskManager::run() {
	server.onHttpRequest([&](const HttpRequest& request) -> HttpResponse {
		Logger::getInstance("Task master", stdout)->write("received: {}", request.getRawUrl());

		if (request.getMethod() == "GET") {
			if (request.getRawUrl() == "/tasks") {
				std::vector<RunningTask> data{};

				for (const auto& task: runningTasks | std::views::values) {
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
	});

	server.onChildRequest([&](const signalfd_siginfo& siginfo) {
		auto logger = Logger::getInstance("Task master", stdout);
		int pid = 1;
		int status;

		switch (siginfo.ssi_signo) {
			case SIGCHLD:
				logger->write("A child is dead");
				while (pid > 0) {
					pid = waitpid(-1, &status, WNOHANG);

					if (pid > 0)
						this->handleDeath(pid, status);
				}
				break;
			case SIGHUP:
				getInstance().loadConf(std::nullopt);
				break;
			case SIGTERM:
			case SIGINT:
				getInstance().stop();
				break;
		}
	});

	server.onWakeUp([&](std::chrono::milliseconds delta) {
		for (auto taskId: stoppingTasks) {
			if (runningTasks[taskId].decreaseStopTime(delta)) {
				kill(runningTasks[taskId]._pid, SIGKILL);
			}
		}
	});

	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGCHLD);
	sigaddset(&set, SIGHUP);
	sigaddset(&set, SIGTERM);
	sigaddset(&set, SIGINT);
	pthread_sigmask(SIG_BLOCK, &set, nullptr);

	server.registerSocket(std::make_shared<SignalSocket>(server, -1, &set));
	startPrograms();
	server.run();
}

void TaskManager::startPrograms() {
	auto logger = Logger::getInstance("Task master", stdout);
	for (const auto& [name,task]: tasksConfs) {
		int pid = fork();
		if (pid == 0) {
			execle("/bin/bash", "bash", "-c", task.cmd.c_str(), nullptr, environ);
		} else {
			auto id = RunningTaskId(name, 0);
			runningTasks[id] = RunningTask(pid);
			logger->write("Launching program: {}", name);
		}
	}
}

TaskManager::TaskManager() {}

TaskManager::~TaskManager() = default;


HttpResponse TaskManager::_getTaskDetails(const HttpRequest& request) {
	if (request.getUrl().size() == 2) {
		auto name = request.getUrl()[1];
		std::vector<RunningTask> tasks{};

		for (const auto& [id, task]: runningTasks) {
			if (id._name == name)
				tasks.push_back(task);
		}

		return {stackixx::serialize(tasks)};
	}

	if (request.getUrl().size() == 3) {
		auto name = request.getUrl()[1];
		auto index = std::stoi(request.getUrl()[2]);

		std::vector<RunningTask> tasks{};

		for (const auto& [id, task]: runningTasks) {
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

	for (auto& [id, task]: runningTasks) {
		if (id._name == name && (task.status == running || task.status == starting)) {
			auto sig = tasksConfs[id._name].getStopSig();
			task.setStopTime(tasksConfs[id._name].getStopTime());
			stoppingTasks.insert(id);
			kill(task._pid, sig);
		}
	}

	return {""};
}

void TaskManager::confHttpServer(ServerConf conf) {
	server.loadConf(conf);
}
