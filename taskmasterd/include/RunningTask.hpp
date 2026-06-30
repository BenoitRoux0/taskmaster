#ifndef RUNNING_TASK_HPP
#define RUNNING_TASK_HPP
#include <chrono>

enum deathStatus {
	stopped,
	starting,
	running,
	stopping,
	restarting,
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
	pid_t   _pid{-1};
	int     status{running};
	int32_t procStatus{0};

	std::chrono::time_point<std::chrono::local_t, std::chrono::nanoseconds> start;
	std::chrono::time_point<std::chrono::local_t, std::chrono::nanoseconds> end;

	void setStopTime(std::chrono::milliseconds ms);
	bool decreaseStopTime(std::chrono::milliseconds ms);

private:
	std::chrono::milliseconds remainingStopTime{-1};
};

#endif // RUNNING_TASK_HPP
