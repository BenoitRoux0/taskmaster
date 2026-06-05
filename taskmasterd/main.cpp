#include <print>
#include "TaskManager.hpp"

int main() {
    std::println("Hello from daemon: {}", getpid());

	TaskManager::getInstance().loadConf("./conf.toml");
	TaskManager::getInstance().run();

	return 0;
}
