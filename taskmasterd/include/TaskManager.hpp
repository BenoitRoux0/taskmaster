#ifndef TASK_MANAGER_HPP
#define TASK_MANAGER_HPP

#include "ServerConf.hpp"
#include "TaskConf.hpp"

#include <toml.hpp>

#include "HttpServer.hpp"

class TaskManager {
	static TaskManager _instance;

public:
	std::map<std::string, TaskConf> tasksConfs;

	void loadConf(const std::optional<std::string>& confFile);
	void stop();

	static TaskManager& getInstance() { return _instance; }

	void run();

private:
	TaskManager();

	~TaskManager();

	HttpServer  server;
	std::string _conf;

	void confHttpServer(ServerConf conf);
};


#endif // TASK_MANAGER_HPP
