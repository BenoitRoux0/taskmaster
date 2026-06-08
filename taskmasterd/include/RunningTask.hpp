#ifndef RUNNING_TASK_HPP
#define RUNNING_TASK_HPP
#include <chrono>

enum deathStatus {
	starting,
	running,
	expected,
	unexpected,
};

class RunningTask {
public:
	RunningTask();

	RunningTask(pid_t pid);

	std::chrono::time_point<std::chrono::local_t, std::chrono::nanoseconds> getStart() const;
	std::chrono::time_point<std::chrono::local_t, std::chrono::nanoseconds> getEnd() const;

	void dead();

// private:
	pid_t   _pid = 0;
	int     status{running};
	int32_t procStatus;

	std::chrono::time_point<std::chrono::local_t, std::chrono::nanoseconds> start;
	std::chrono::time_point<std::chrono::local_t, std::chrono::nanoseconds> end;
};

#endif // RUNNING_TASK_HPP
