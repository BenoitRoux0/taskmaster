#include <fcntl.h>
#include <iostream>
#include "TaskManager.hpp"
#include "tm_common.hpp"

int main() {
	TaskManager manager(false);
	try {
		manager.loadConf(getEnv("TM_CONF","./conf.toml"));
		manager.run();
	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}

	return 0;
}
