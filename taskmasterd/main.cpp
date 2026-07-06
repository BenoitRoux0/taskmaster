#include <fcntl.h>
#include <iostream>
#include <print>
#include "TaskManager.hpp"
#include "tm_common.hpp"

int main() {
	TaskManager manager(true);
	try {
		manager.loadConf(getEnv("TM_CONF","./conf.toml"));
		manager.run();
	} catch (const std::exception& e) {
		std::println(stderr, "Error: {}", e.what());
	}

	return 0;
}
