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

void RunningTask::dead() {
	end = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
}
