#ifndef RUNNING_TASK_HPP
#define RUNNING_TASK_HPP
#include <sys/types.h>

enum deathStatus {
	starting,
	running,
	expected,
	unexpected,
};

class RunningTask {
public:
	RunningTask() = default;

	RunningTask(pid_t pid);

	pid_t _pid = 0;
	int     status{running};
};

#endif // RUNNING_TASK_HPP
