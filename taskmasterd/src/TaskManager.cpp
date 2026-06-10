#include "TaskManager.hpp"

#include <csignal>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <vector>
#include <print>
#include <ranges>
#include <string>

#include "Logger.hpp"
#include "serializer.hpp"
#include "SignalSocket.hpp"
#include "TaskData.hpp"
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
		} else if (request.getUrl().size() == 3 && request.getUrl()[0] == "task" && request.getUrl()[2] == "start") {
			return this->_startTask(request);
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

static void configureTask(TaskConf task) {
	if (task.umask.has_value()) {
		umask(task.umask.value());
	}

	if (task.workdir.has_value()) {
		if (chdir(task.workdir.value().c_str()) == -1) {
			perror(("chdir to " + task.workdir.value()).c_str());
			exit(1);
		}
	}

	if (task.env.has_value()) {
		for (const auto& [key, value]: task.env.value()) {
			setenv(key.c_str(), value.c_str(), 1);
		}
	}

	if (task.std_in.has_value()) {
		int fd = open(task.std_in.value().c_str(), O_RDONLY);
		if (fd == -1) {
			perror(("open stdin for " + task.std_in.value()).c_str());
			exit(1);
		}
		dup2(fd, STDIN_FILENO);
		close(fd);
	}

	if (task.std_out.has_value()) {
		int fd = open(task.std_out.value().c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
		if (fd == -1) {
			perror(("open stdout for " + task.std_out.value()).c_str());
			exit(1);
		}
		dup2(fd, STDOUT_FILENO);
		close(fd);
	}

	if (task.std_err.has_value()) {
		int fd = open(task.std_err.value().c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
		if (fd == -1) {
			perror(("open stderr for " + task.std_err.value()).c_str());
			exit(1);
		}
		dup2(fd, STDERR_FILENO);
		close(fd);
	}
}

void TaskManager::startPrograms() {
	for (auto& [name, task]: _tasksConfs) {
		if (!task.getStartAtLaunch()) {
			continue;
		}

		int num_procs = task.getNumProcs();
		for (int i = 0; i < num_procs; ++i) {
			startProgram(name, i);
		}
	}
}

void TaskManager::startProgram(const std::string& name, int index) {
	auto confIt = _tasksConfs.find(name);

	if (confIt == _tasksConfs.end()) {
		_logger.write("Cannot start '{}': program not found in configuration", name);
		return;
	}

	if (index < 0 || index >= confIt->second.getNumProcs()) {
		_logger.write("Cannot start '{}': invalid process index {}", name, index);
		return;
	}

	RunningTaskId id(name, index);
	if (_runningTasks.find(id) != _runningTasks.end()) {
		if (_runningTasks[id].status == running || _runningTasks[id].status == starting) {
			_logger.write("Program already running: {}", name);
			return;
		}
	}

	startTask(name, index, confIt->second);
}

void TaskManager::startTask(const std::string& name, int index, const TaskConf& taskConf) {
	RunningTaskId id(name, index);

	pid_t pid = fork();
	if (pid == 0) {
		configureTask(taskConf);
		std::string shell = taskConf.getShell();

		execle(shell.c_str(), shell.c_str(), "-c", taskConf.cmd.c_str(), nullptr, environ);

		perror("execle");
		exit(1);
	} else if (pid > 0) {
		_runningTasks[id] = RunningTask(pid);
		_runningTasks[id].status = starting;
		_logger.write("Launching program: {}_{}", name, index);
	} else {
		perror("fork");
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
		auto name = request.getUrl()[1];
		std::vector<TaskData> tasks{};

		for (const auto& [id, task]: _runningTasks) {
			if (id._name == name)
				tasks.push_back({
					id._name,
					id._index,
					task.status,
					task.procStatus,
					_tasksConfs[id._name].cmd
				});
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

HttpResponse TaskManager::_startTask(const HttpRequest& request) {
	const auto& url = request.getUrl();

	if (url.size() != 3 || url[0] != "task" || url[2] != "start") {
		return {"400", ""};
	}

	const std::string& name = url[1];
	auto               confIt = _tasksConfs.find(name);

	if (confIt == _tasksConfs.end()) {
		_logger.write("Start request rejected: '{}' is not configured", name);
		return {"404", "program is not configured"};
	}

	bool startedAtLeastOne = false;
	int  alreadyRunning = 0;

	for (int i = 0; i < confIt->second.getNumProcs(); ++i) {
		RunningTaskId id(name, i);
		auto          it = _runningTasks.find(id);

		if (it != _runningTasks.end() && (it->second.status == running || it->second.status == starting)) {
			++alreadyRunning;
			continue;
		}

		startProgram(name, i);
		startedAtLeastOne = true;
	}

	if (!startedAtLeastOne) {
		_logger.write("Start request ignored: '{}' already running ({} process(es))", name, alreadyRunning);
		return {"403", "program already running"};
	}

	return {"Program started"};
}
