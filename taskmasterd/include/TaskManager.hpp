#ifndef TASK_MANAGER_HPP
#define TASK_MANAGER_HPP

#include "ServerConf.hpp"
#include "TaskConf.hpp"

#include <toml.hpp>

#include "RunningTask.hpp"
#include "RunningTaskId.hpp"
#include "Server.hpp"

class TaskManager {
	static TaskManager _instance;

public:
	std::map<std::string, TaskConf>      tasksConfs;
	std::map<RunningTaskId, RunningTask> runningTasks;

	void loadConf(const std::optional<std::string>& confFile);
	void stop();
	void handleDeath(pid_t pid, int32_t status);

	static TaskManager& getInstance() { return _instance; }

	void run();
	void startPrograms();

private:
	TaskManager();

	~TaskManager();

	Server      server;
	std::string _conf;

	HttpResponse _getTaskDetails(const HttpRequest& request);
	HttpResponse _stopTask(const HttpRequest& request);

	void confHttpServer(ServerConf conf);
};

#endif // TASK_MANAGER_HPP
