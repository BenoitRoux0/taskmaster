#ifndef TASK_CONF_HPP
#define TASK_CONF_HPP
#include <map>
#include <optional>
#include <toml.hpp>

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
