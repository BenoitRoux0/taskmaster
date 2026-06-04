#include "TaskManager.hpp"

#include <csignal>
#include <print>
#include <ranges>

#include "common.hpp"
#include "serializer.hpp"
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
	server.onRequest([&](const HttpRequest& request) -> HttpResponse {
		std::println("received: {}", request.getUrl());

		std::vector<taskData> data{};

		for (const auto& task: tasksConfs | std::views::values) {
			data.push_back({task.cmd});
		}

		return {stackixx::serialize(data)};
	});
	server.run();
}

TaskManager::TaskManager() {
	signal(SIGHUP, []([[maybe_unused]] int sig) -> void {
		getInstance().loadConf(std::nullopt);
		std::println("Handle: {}", sig);
	});

	signal(SIGTERM, []([[maybe_unused]] int sig) -> void {
		getInstance().stop();
		std::println("Handle: {}", sig);
	});

	signal(SIGINT, []([[maybe_unused]] int sig) -> void {
		getInstance().stop();
		std::println("Handle: {}", sig);
	});
}

TaskManager::~TaskManager() {
	std::println("destroy manager");
}

void TaskManager::confHttpServer(ServerConf conf) {
	server.loadConf(conf);
}
