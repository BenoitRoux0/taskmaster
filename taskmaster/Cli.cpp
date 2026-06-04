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
            auto exitCode = handleCommand(cmd);
            if (exitCode.has_value()) {
                return *exitCode;
            }
        }
        catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
        }}
    return 0;
}

std::optional<int> Cli::handleCommand(const Command& cmd) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL");
    }
    HttpClient client(cmd, curl);

    switch (cmd.type) {
        case commandType::NOTHING:
            break;
        case commandType::EXIT:
            return 0;
        case commandType::STATUS:
            if (!cmd.args.empty()) {
                for (const std::string& arg : cmd.args) {
                    client.post("/status", "{\"name\":\"" + arg + "\"}");
                }
            }
            else {
                client.get("/status");
            }
            break;
        case commandType::START:
            if (!cmd.args.empty()) {
                for (const std::string& arg : cmd.args) {
                    client.post("/start", "{\"id\":\"" + arg + "\"}");
                }
            }
            else {
                std::println("Start requires a process name");
            }
            break;
        case commandType::STOP:
            if (!cmd.args.empty()) {
                for (const std::string& arg : cmd.args) {
                    client.post("/stop", "{\"id\":\"" + arg + "\"}");
                }
            }
            else {
                std::println("Stop requires a process name");
            }
            break;
        case commandType::RESTART:
            if (!cmd.args.empty()) {
                for (const std::string& arg : cmd.args) {
                    client.post("/restart", "{\"id\":\"" + arg + "\"}");
                }
            }
            else {
                std::println("Restart requires a process name");
            }
            break;
        case commandType::RELOAD:
            if (cmd.args.empty()) {
                client.post("/reload", "");
            }
            else {
                std::println("Reload does not take any arguments");
            }
            break;
    }

    return std::nullopt;
}
