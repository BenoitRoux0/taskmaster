#ifndef TASK_MANAGER_HPP
#define TASK_MANAGER_HPP

#include "ServerConf.hpp"
#include "TaskConf.hpp"

#include <toml.hpp>

#include "Logger.hpp"
#include "RunningTask.hpp"
#include "RunningTaskId.hpp"
#include "Server.hpp"

class TaskManager {
public:
	TaskManager();

	~TaskManager();

	void loadConf(const std::optional<std::string>& confFile);
	void reloadConf(const std::optional<std::string>& confFile);
	void stop();
	void handleDeath(pid_t pid, int32_t status);
	void stopAndRemove(const std::string& name, const TaskConf& conf);

	void run();
	void startPrograms();
	void startProgram(const std::string& name, int index);

	HttpResponse _onHttpRequest(const HttpRequest& request);
	void         _onChildRequest(const signalfd_siginfo& siginfo);
	void         _onWakeUp(std::chrono::milliseconds delta);

private:
	Server      _server;
	std::string _conf;

	HttpResponse _getTaskDetails(const HttpRequest& request);
	HttpResponse _stopTask(const HttpRequest& request);
	HttpResponse _startTask(const HttpRequest& request);
	HttpResponse _reloadConf(const HttpRequest& request);

	Logger _logger{};

	std::set<RunningTaskId>					_stoppingTasks{};
	std::map<std::string, TaskConf>			_tasksConfs{};
	std::map<RunningTaskId, RunningTask>	_runningTasks{};

	void startTask(const std::string& name, int index, const TaskConf& taskConf);
};

#endif // TASK_MANAGER_HPP
