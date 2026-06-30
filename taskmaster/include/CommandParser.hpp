#ifndef TASKMASTER_COMMANDPARSER_HPP
# define TASKMASTER_COMMANDPARSER_HPP

#include <expected>
# include <string>
# include <vector>

enum class commandType {
	HELP,
	STATUS,
	START,
	STOP,
	RELOAD,
	RESTART,
	EXIT,
	NOTHING,
};

struct Command {
	commandType              type;
	std::vector<std::string> args;
};

class CommandParser {
public:
	CommandParser();
	~CommandParser();

	std::expected<Command, std::string> parseInput(const std::string& input);

private:
};


#endif //TASKMASTER_COMMANDPARSER_HPP
