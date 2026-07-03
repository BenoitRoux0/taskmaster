#include <fcntl.h>
#include <print>
#include "TaskManager.hpp"

int main() {
	TaskManager manager(false);

	manager.loadConf("./conf.toml");
	manager.run();

	return 0;
}
