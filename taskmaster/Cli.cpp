#include "Cli.hpp"

Cli::Cli() {
}

Cli::~Cli() {
}

int Cli::run() {
    using_history();
    while (true) {
        char*   input;

        input = readline("Taskmaster> ");
        if (!input) {
            break;
        }
        std::string line(input);
        add_history(input);
        free(input);

        try {
            Command cmd = _parser.parseInput(line);
            if (cmd.type == commandType::EXIT) {
                break;
            }
            CURL* curl = curl_easy_init();
            HttpClient client(cmd, curl);
        }
        catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
        }}
    return 0;
}