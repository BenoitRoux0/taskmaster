#include "CommandParser.hpp"
#include <print>
#include <string>
#include <sstream>

CommandParser::CommandParser() {
}

CommandParser::~CommandParser() {
}

static std::string trim(const std::string& input) {
	constexpr auto	whitespace = " \t\n\r\f\v";
	std::string		str2 = input;

	str2.erase(0, input.find_first_not_of(whitespace));
	str2.erase(input.find_last_not_of(whitespace) + 1);

	return str2;
}

std::expected<Command, std::string> CommandParser::parseInput(const std::string& input) {
	Command		cmd{};
	std::string trimmed = trim(input);
	if (trimmed.empty()) {
		cmd.type = commandType::NOTHING;
		return cmd;
	}

	std::istringstream	iss(trimmed);
	std::string			arg;

	if (!(iss >> arg)) {
		cmd.type = commandType::NOTHING;
		return cmd;
	}

	std::string commandStr = arg;
	while (iss >> arg) {
		cmd.args.push_back(arg);
	}

	if (commandStr == "help") {
		cmd.type = commandType::HELP;
	} else if (commandStr == "status") {
		cmd.type = commandType::STATUS;
	} else if (commandStr == "start") {
		cmd.type = commandType::START;
	} else if (commandStr == "stop") {
		cmd.type = commandType::STOP;
	} else if (commandStr == "reload") {
		cmd.type = commandType::RELOAD;
	} else if (commandStr == "restart") {
		cmd.type = commandType::RESTART;
	} else if (commandStr == "exit") {
		cmd.type = commandType::EXIT;
	} else {
		return std::unexpected(std::format("Unknown command: {}", commandStr));
	}

	return cmd;
}