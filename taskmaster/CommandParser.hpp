#ifndef TASKMASTER_COMMANDPARSER_HPP
# define TASKMASTER_COMMANDPARSER_HPP

# include <string>
# include <stdexcept>

enum class commandType {
    STATUS,
    START,
    STOP,
    RELOAD,
    RESTART,
    EXIT,
    NOTHING,
};

struct Command {
    commandType type;
    std::string args;
};

class CommandParser {
    public:
        CommandParser();
        ~CommandParser();

        Command parseInput(const std::string& input);

    private:
};


#endif //TASKMASTER_COMMANDPARSER_HPP