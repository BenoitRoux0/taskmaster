#include "RunningTask.hpp"
RunningTask::RunningTask(pid_t pid): _pid(pid) {
}

std::chrono::time_point<std::chrono::steady_clock> RunningTask::getStart() {
	return start;
}
