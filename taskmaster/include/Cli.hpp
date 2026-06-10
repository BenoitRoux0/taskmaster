#ifndef TASKMASTER_CLI_H
# define TASKMASTER_CLI_H

#include "CommandParser.hpp"
#include "HttpClient.hpp"
#include "ClientConfig.hpp"

# include <iostream>
# include <string>
# include <print>
# include <exception>
# include <optional>
# include <readline/readline.h>
# include <readline/history.h>

class Cli {
    public:
        Cli(const ClientConfig& config = ClientConfig());
        ~Cli();

        int run();

    private:
        std::optional<int> handleCommand(const Command& cmd);

		std::pair<>

        CommandParser   _parser;
        HttpClient      _client;
};


#endif //TASKMASTER_CLI_H