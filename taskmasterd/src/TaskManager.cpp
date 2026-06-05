#include "TaskManager.hpp"

#include <csignal>
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

void TaskManager::handleDeath(pid_t pid) {
	for (auto& task: runningTasks | std::views::values) {
		if (task._pid == pid) {
			task.dead = true;
		}
	}
}

void TaskManager::run() {
	server.onHttpRequest([&](const HttpRequest& request) -> HttpResponse {
		Logger::getInstance("Task master", stdout)->write("received: {}", request.getUrl());

		std::vector<RunningTask> data {};

		for (const auto& task: runningTasks | std::views::values) {
			data.push_back(task);
		}

		return {stackixx::serialize(data)};
	});

	server.onChildRequest([&](const signalfd_siginfo& siginfo) {
		auto logger = Logger::getInstance("Task master", stdout);

		switch (siginfo.ssi_signo) {
			case SIGCHLD:
				logger->write("A child is dead");
				this->handleDeath(siginfo.ssi_pid);
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
	auto logger=Logger::getInstance("Task master", stdout);
	for (auto [name,task]:tasksConfs) {
		int pid = fork();
		if (pid == 0){
			execle("/bin/bash", "bash", "-c", task.cmd.c_str(), nullptr, environ);
		}
		else {
			auto id = RunningTaskId(name, pid);
			runningTasks[id] = RunningTask(pid);
			logger->write("Launching program: {}", name);
		}
	}
}

TaskManager::TaskManager() {}

TaskManager::~TaskManager() = default;

void TaskManager::confHttpServer(ServerConf conf) {
	server.loadConf(conf);
}
