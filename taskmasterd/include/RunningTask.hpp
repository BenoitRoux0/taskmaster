#ifndef RUNNING_TASK_HPP
#define RUNNING_TASK_HPP
#include <chrono>

#include "State.hpp"

class RunningTask {
public:
	RunningTask();

	RunningTask(pid_t pid);

	std::chrono::time_point<std::chrono::local_t, std::chrono::nanoseconds> getStart() const;
	std::chrono::time_point<std::chrono::local_t, std::chrono::nanoseconds> getEnd() const;

	void dead(std::chrono::time_point<std::chrono::local_t, std::chrono::nanoseconds> time);

	// private:
	pid_t   _pid{-1};
	State   status{State::running};
	int32_t procStatus{0};


	void setStopTime(std::chrono::milliseconds ms);
	bool decreaseStopTime(std::chrono::milliseconds ms);

private:
	std::chrono::time_point<std::chrono::local_t, std::chrono::nanoseconds> start;
	std::chrono::time_point<std::chrono::local_t, std::chrono::nanoseconds> end;

	std::chrono::milliseconds remainingStopTime{-1};
};

#endif // RUNNING_TASK_HPP
