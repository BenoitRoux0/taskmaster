#ifndef TASKMASTER_CLI_H
# define TASKMASTER_CLI_H

# include "CommandParser.hpp"
# include "HttpClient.hpp"

# include <iostream>
# include <string>
# include <print>
# include <exception>
# include <optional>
# include <readline/readline.h>
# include <readline/history.h>
# include <curl/curl.h>

class Cli {
    public:
        Cli();
        ~Cli();

        int run();

    private:
        std::optional<int> handleCommand(const Command& cmd);

    CommandParser   _parser;
    HttpClient      _client;
};


#endif //TASKMASTER_CLI_H