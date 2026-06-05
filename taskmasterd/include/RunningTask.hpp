#ifndef RUNNING_TASK_HPP
#define RUNNING_TASK_HPP
#include <sys/types.h>

class RunningTask {
public:
	RunningTask() = default;

	RunningTask(pid_t pid);

	pid_t _pid = 0;
};

#endif // RUNNING_TASK_HPP
