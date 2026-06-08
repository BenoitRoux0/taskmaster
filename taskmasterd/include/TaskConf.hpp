#ifndef TASK_CONF_HPP
#define TASK_CONF_HPP
#include <map>
#include <optional>
#include <toml.hpp>
#include <csignal>

struct TaskConf {
	std::string                                       cmd;
	std::optional<int>                                num_procs;
	std::optional<bool>                               start_at_launch;
	std::optional<std::string>                        restart;
	std::optional<std::vector<int>>                   exit_codes;
	std::optional<int>                                start_time;
	std::optional<int>                                retries;
	std::optional<int>                                stop_sig;
	std::optional<int>                                stop_time;
	std::optional<std::string>                        std_in;
	std::optional<std::string>                        std_out;
	std::optional<std::string>                        workdir;
	std::optional<mode_t>                             umask;
	std::optional<std::string>                        shell;
	std::optional<std::map<std::string, std::string>> env;

	const std::string	getCmd() const { return cmd; };
	int					getNumProcs() const { return num_procs.value_or(1);};
	bool				getStartAtLaunch() const { return start_at_launch.value_or(false);};
	std::string			getRestart() const { return restart.value_or("unexpected");};
	std::vector<int>	getExitCodes() const { return exit_codes.value_or({0});};
	int					getStartTime() const { return start_time.value_or(1);};
	int					getRetries() const { return retries.value_or(3);};
	int					getStopSig() const { return stop_sig.value_or(SIGTERM);};
	int					getStopTime() const { return stop_time.value_or(10);};
	std::string			getStdIn() const { return std_in.value_or("");};
	std::string			getStdOut() const { return std_out.value_or("");};
	std::string			getWorkDir() const { return workdir.value_or("");};
	mode_t				getUmask() const { return umask.value_or(022);};
	std::string			getShell() const { return shell.value_or("/bin/bash");};
	std::map<std::string, std::string> getEnv() const { return env.value_or({});};
};

TOML11_DEFINE_CONVERSION_NON_INTRUSIVE(TaskConf,
                                       cmd,
                                       num_procs,
                                       start_at_launch,
                                       restart,
                                       exit_codes,
                                       start_time,
                                       retries,
                                       stop_sig,
                                       stop_time,
                                       std_in,
                                       std_out,
                                       workdir,
                                       umask,
                                       shell,
                                       env
)

#endif // TASK_CONF_HPP
