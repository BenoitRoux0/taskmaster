#include "TaskManager.hpp"
#include <csignal>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <vector>
#include <cstring>
#include <print>
#include <ranges>

#include "Logger.hpp"
#include "serializer.hpp"
#include "SignalSocket.hpp"
#include "../../common/common.h"

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

void TaskManager::run() {
	server.onHttpRequest([&](const HttpRequest& request) -> HttpResponse {
		Logger::getInstance("Task master", stdout)->write("received: {}", request.getUrl());

		std::vector<taskData> data{};

		for (const auto& task: tasksConfs | std::views::values) {
			data.push_back({task.cmd});
		}

		return {stackixx::serialize(data)};
	});

	server.onChildRequest([&](const signalfd_siginfo& siginfo) {
		Logger::getInstance("Task master", stdout)->write("received: {}", siginfo.ssi_signo);

		switch (siginfo.ssi_signo) {
			case SIGHUP:
				getInstance().loadConf(std::nullopt);
				break;
			case SIGTERM:
			case SIGINT:
				getInstance().stop();
				break;
		}
	});

	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);
	sigaddset(&set, SIGHUP);
	sigaddset(&set, SIGTERM);
	sigaddset(&set, SIGINT);
	pthread_sigmask(SIG_BLOCK, &set, nullptr);

	server.registerSocket(std::make_shared<SignalSocket>(server, -1, &set));
	startPrograms();
	server.run();
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
		for (const auto& [key, value] : task.env.value()) {
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
	auto logger = Logger::getInstance("Task master", stdout);
	for (auto& [name, task] : tasksConfs) {
		if (!task.start_at_launch.value()) {
			continue;
		}

		int num_procs = task.getNumProcs();
		for (int i = 0; i < num_procs; ++i) {
			pid_t pid = fork();
			if (pid == 0) {
				configureTask(task);
				std::string shell = task.getShell();

				execle(shell.c_str(), shell.c_str(), "-c", task.cmd.c_str(), nullptr, environ);
				
				perror("execle");
				exit(1);
			} else if (pid > 0) {
				auto id = RunningTaskId(name, i);
				runningTasks[id] = RunningTask(pid);
				logger->write("Launching program: {}_{}", name, i);
			} else {
				perror("fork");
			}
		}
	}
}

TaskManager::TaskManager() {}

TaskManager::~TaskManager() = default;

void TaskManager::confHttpServer(ServerConf conf) {
	server.loadConf(conf);
}
