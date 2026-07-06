#include "Task.hpp"

#include <csignal>

Task::Task(const TaskConf& conf) {
	command = conf.cmd;
	numProcs = conf.num_procs.value_or(1);
	startAtLaunch = conf.start_at_launch.value_or(false);
	restart = restartPolicyFromStr(conf.restart.value_or("never"));
	exitCodes = conf.exit_codes.value_or({0});
	startTime = conf.start_time.value_or(0);
	retries = conf.retries.value_or(0);
	stopSig = conf.stop_sig.value_or(SIGTERM);
	stopTime = conf.stop_time.value_or(1);
	stdIn = conf.std_in.value_or("");
	stdOut = conf.std_out.value_or("");
	workDir = conf.workdir.value_or("");
	umask = conf.umask.value_or(0);
}
