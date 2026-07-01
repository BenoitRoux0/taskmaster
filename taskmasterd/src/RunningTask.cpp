#include "RunningTask.hpp"

RunningTask::RunningTask() {
	start = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
}

RunningTask::RunningTask(pid_t pid): _pid(pid) {
	start = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
}

std::chrono::time_point<std::chrono::local_t, std::chrono::nanoseconds> RunningTask::getStart() const {
	return start;
}

std::chrono::time_point<std::chrono::local_t, std::chrono::nanoseconds> RunningTask::getEnd() const {
	return end;
}

void RunningTask::dead(std::chrono::time_point<std::chrono::local_t, std::chrono::nanoseconds> time) {
	end = time;
}

void RunningTask::setStopTime(const std::chrono::milliseconds ms) {
	remainingStopTime = ms;
}

bool RunningTask::decreaseStopTime(const std::chrono::milliseconds ms) {
	if (ms > remainingStopTime) {
		remainingStopTime = std::chrono::milliseconds(0);
		return true;
	}

	remainingStopTime -= ms;
	return false;
}
