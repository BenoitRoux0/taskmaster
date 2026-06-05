#ifndef TASK_HPP
#define TASK_HPP

#include <string>
#include <vector>

#include "RestartPolicy.hpp"
#include "TaskConf.hpp"


class Task {
	std::string      command;
	int              numProcs;
	bool             startAtLaunch;
	RestartPolicy    restart;
	std::vector<int> exitCodes;
	int              startTime;
	int              retries;
	int              stopSig;
	int              stopTime;
	std::string      stdIn;
	std::string      stdOut;
	std::string      workDir;
	mode_t           umask;
	std::string      shell;

public:
	explicit Task(const TaskConf& conf);
};

#endif // TASK_HPP
