#ifndef RUNNING_TASK_HPP
#define RUNNING_TASK_HPP
#include <chrono>
#include <sys/types.h>

enum deathStatus {
	running,
	expected,
	unexpected,
};

class RunningTask {
public:
	RunningTask() = default;

	RunningTask(pid_t pid);

	pid_t                                              _pid = 0;
	int                                                death{running};

	std::chrono::time_point<std::chrono::steady_clock> getStart();
private:
	std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();
};

#endif // RUNNING_TASK_HPP
