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

	static TaskManager& getInstance() { return _instance; }

	void run();
	void startPrograms();
	void startProgram(const std::string& name, int index);

private:
	TaskManager();

	~TaskManager();

	Server      server;
	std::string _conf;

	void confHttpServer(ServerConf conf);

	void startTask(const std::string& name, int index, const TaskConf& taskConf);

	HttpResponse _getTaskDetails(const HttpRequest& request);
	HttpResponse _stopTask(const HttpRequest& request);
	HttpResponse _startTask(const HttpRequest& request);

};


#endif // TASK_MANAGER_HPP
