#include "Cli.hpp"

#include <algorithm>
#include <expected>

#include "deserializer.hpp"
#include "serializer.hpp"

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

        auto cmd = _parser.parseInput(line);
    	if (!cmd.has_value()) {
    		std::println(stderr, "{}", cmd.error());
    		continue;
    	}
    	auto exitCode = handleCommand(cmd.value());
        if (exitCode.has_value()) {
            return *exitCode;
        }
    }
    return 0;
}

std::optional<int> Cli::handleCommand(const Command& cmd) {
	std::expected<std::string, std::string> res;

    switch (cmd.type) {
        case commandType::EXIT:
            if (!cmd.args.empty() && cmd.args.size() == 1) {
                if (cmd.args[0] == "cli") {
                    return 0;
                } else if (cmd.args[0] == "daemon") {
                    response = _client.post("exit", "");
                    std::cout << response << std::endl;
                } else {
                    std::println("Exit requires one argument: 'daemon' or 'cli'");
                }
            } else {
                std::println("Exit requires either 'daemon' or 'cli'");
            }
            break;
        case commandType::STATUS:
			handleStatus(cmd);
            break;
        case commandType::START:
            if (!cmd.args.empty()) {
                for (const std::string& arg : cmd.args) {
                    res = _client.post<std::string>("/start/{}", arg);
                    std::cout << res.value_or(res.error()) << std::endl;
                }
            }
            else {
                std::println("Start requires a process name");
            }
            break;
        case commandType::STOP:
            if (!cmd.args.empty()) {
                for (const std::string& arg : cmd.args) {
                    res = _client.post<std::string>("/stop/{}",arg );
                    std::cout << res.value_or(res.error()) << std::endl;
                }
            }
            else {
                std::println("Stop requires a process name");
            }
            break;
        case commandType::RESTART:
            if (!cmd.args.empty()) {
                for (const std::string& arg : cmd.args) {
                    res = _client.post<std::string>("/restart/{}", arg);
                    std::cout << res.value_or(res.error()) << std::endl;
                }
            }
            else {
                std::println("Restart requires a process name");
            }
            break;
        case commandType::RELOAD:
            if (cmd.args.empty()) {
                res = _client.post<std::string>("/reload");
                std::cout << res.value_or(res.error()) << std::endl;
            }
            else {
                std::println("Reload does not take any arguments");
            }
            break;
		default: break;
    }

    return std::nullopt;
}

std::optional<int> Cli::handleStatus(const Command& cmd) {
	std::vector<TaskData> res;
	std::map<std::string, std::vector<TaskData>> tasks;

	if (!cmd.args.empty()) {
		for (const std::string& arg : cmd.args) {
			auto current = _client.get<std::vector<TaskData>>( "task/{}", arg);

			if (current.has_value()) {
				res.append_range(current.value());
			}
		}
	} else {
		auto current = _client.get<std::vector<TaskData>>("tasks");
		if (current.has_value())
			res.append_range(current.value());
	}

	for (const auto& task: res) {
		tasks[task.name].push_back(task);
	}

	for (const auto& [name, subTasks]: tasks) {
		std::println("{}", name);
		for (std::string start = " ├─ "; const auto& task: subTasks) {
		    if (task.index == subTasks.size() - 1)
			    start = " └─ ";

			std::println("{}{}.{}", start, task.name, task.index);
		}
	}

	return std::nullopt;
}

std::expected<std::vector<TaskData>, std::string> Cli::getTask(const std::string& name) {
	return _client.get<std::vector<TaskData>>("task/{}", name);
}
