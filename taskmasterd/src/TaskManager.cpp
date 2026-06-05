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

	server.run();
}

TaskManager::TaskManager() {}

TaskManager::~TaskManager() = default;

void TaskManager::confHttpServer(ServerConf conf) {
	server.loadConf(conf);
}
