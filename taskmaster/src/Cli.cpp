#include "Cli.hpp"

Cli::Cli(const ClientConfig& config) : _client(config) {
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
            auto exitCode = handleCommand(cmd);
            if (exitCode.has_value()) {
                return *exitCode;
            }
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
    }
    return 0;
}

std::optional<int> Cli::handleCommand(const Command& cmd) {
    std::string response ;
    switch (cmd.type) {
        case commandType::NOTHING:
            break;
        case commandType::EXIT:
            return 0;
        case commandType::STATUS:
            if (!cmd.args.empty()) {
                for (const std::string& arg : cmd.args) {
                    response = _client.get("task", arg);
                    std::cout << response << std::endl;
                }
            }
            else {
                response = _client.get("tasks", "");
                std::cout << response << std::endl;
            }
            break;
        case commandType::START:
            if (!cmd.args.empty()) {
                for (const std::string& arg : cmd.args) {
                    response = _client.post("start", arg);
                    std::cout << response << std::endl;
                }
            }
            else {
                std::println("Start requires a process name");
            }
            break;
        case commandType::STOP:
            if (!cmd.args.empty()) {
                for (const std::string& arg : cmd.args) {
                    response = _client.post("stop",arg );
                    std::cout << response << std::endl;
                }
            }
            else {
                std::println("Stop requires a process name");
            }
            break;
        case commandType::RESTART:
            if (!cmd.args.empty()) {
                for (const std::string& arg : cmd.args) {
                    response = _client.post("restart", "{\"id\":\"" + arg + "\"}");
                    std::cout << response << std::endl;
                }
            }
            else {
                std::println("Restart requires a process name");
            }
            break;
        case commandType::RELOAD:
            if (cmd.args.empty()) {
                response = _client.post("reload", "");
                std::cout << response << std::endl;
            }
            else {
                std::println("Reload does not take any arguments");
            }
            break;
    }

    return std::nullopt;
}
