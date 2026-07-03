#include "TaskManager.hpp"
#include "tm_common.hpp"

int main() {
	TaskManager manager(false);

	manager.loadConf(getEnv("TM_CONF", "./conf.toml"));
	manager.run();

	return 0;
}
