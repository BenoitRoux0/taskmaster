#include "CommandParser.hpp"

CommandParser::CommandParser() {
}

CommandParser::~CommandParser() {
}

Command CommandParser::parseInput(const std::string& input) {
	Command		cmd{};
	std::string trimmed = input;
	trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r\f\v"));
	if (trimmed.empty()) {
		cmd.type = commandType::NOTHING;
		return cmd;
	}
	trimmed.erase(trimmed.find_last_not_of(" \t\n\r\f\v") + 1);

	size_t spacePos = trimmed.find(' ');
	std::string commandStr = (spacePos == std::string::npos) ? trimmed : trimmed.substr(0, spacePos);
	cmd.args = (spacePos == std::string::npos) ? "" : trimmed.substr(spacePos + 1);

	if (commandStr == "status") {
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
		throw std::invalid_argument("Unknown command: " + commandStr);
	}

	return cmd;
}