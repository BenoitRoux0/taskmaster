#include <print>
#include "TaskManager.hpp"

int main() {
    std::println("Hello from daemon: {}", getpid());

	TaskManager manager;

	manager.loadConf("./conf.toml");
	manager.run();

	return 0;
}
