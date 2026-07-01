#ifndef TASKMASTER_CLI_H
# define TASKMASTER_CLI_H

#include "CommandParser.hpp"
#include "HttpClient.hpp"
#include "ClientConfig.hpp"

# include <iostream>
# include <string>
# include <print>
# include <exception>
# include <expected>
# include <optional>
# include <readline/readline.h>
# include <readline/history.h>

#include "TaskData.hpp"

class Cli {
public:
	Cli(const ClientConfig& config = ClientConfig());
	~Cli();

	int run();

private:
	std::optional<int> handleCommand(const Command& cmd);
	template <commandType Ct>
	std::optional<int> handleCommand(const std::vector<std::string>& args);
	std::optional<int> handleStatus(const Command& cmd);
	std::optional<int> handleStart(const Command& cmd);

	std::expected<std::vector<TaskData>, std::string> getTask(const std::string& name);

	CommandParser _parser;
	HttpClient    _client;
};

template <commandType Ct>
std::optional<int> Cli::handleCommand(const std::vector<std::string>& args) {
	std::print("{}: unknown command\n", args[0]);
	return {};
}

template <>
inline std::optional<int> Cli::handleCommand<commandType::HELP>(const std::vector<std::string>& args) {
	std::print("Help menu\n");
	return {};
}

#endif //TASKMASTER_CLI_H
