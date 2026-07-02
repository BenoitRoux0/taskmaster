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

std::optional<int> Cli::handleStart(const Command& cmd) {
	std::expected<std::string, std::string> res;
	if (!cmd.args.empty()) {
		for (const std::string& arg : cmd.args) {
			res = _client.post<std::string>("task/{}/start", arg);
			if (res.has_value()) {
				std::println("{}", res.value());
			} else {
				std::println(stderr, "{}", res.error_or("unknown error"));
			}
		}
	}
	else {
		std::println("Start requires a process name");
	}

	return {};
}

void Cli::handleReload(const Command& cmd) {
	if (cmd.args.empty()) {
		auto res = _client.post<std::string>("reload");
		if (res.has_value()) {
			std::println("{}", res.value());
		} else {
			std::println(stderr, "{}", res.error_or("unknown error"));
		}
	}
	else {
		std::println("Reload does not take any arguments");
	}
}

std::optional<int> Cli::handleCommand(const Command& cmd) {
	std::expected<std::string, std::string> res;

    switch (cmd.type) {
        case commandType::EXIT:
            if (!cmd.args.empty() && cmd.args.size() == 1) {
                if (cmd.args[0] == "cli") {
                    return 0;
                } else if (cmd.args[0] == "daemon") {
                	res = _client.post<std::string>("/exit");
                	std::cout << res.value_or(res.error()) << std::endl;
                } else {
                    std::println("Exit requires one argument: 'daemon' or 'cli'");
                }
            } else {
                std::println("Exit requires either 'daemon' or 'cli'");
            }
            break;
        case commandType::STATUS:
			return handleStatus(cmd);
            break;
        case commandType::START:
            return handleStart(cmd);
            break;
        case commandType::STOP:
            if (!cmd.args.empty()) {
                for (const std::string& arg : cmd.args) {
                    res = _client.post<std::string>("task/{}/stop",arg );
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
                    res = _client.post<std::string>("task/{}/restart", arg);
                    std::cout << res.value_or(res.error()) << std::endl;
                }
            }
            else {
                std::println("Restart requires a process name");
            }
            break;
        case commandType::RELOAD:
            handleReload(cmd);
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
			} else {
				std::println(stderr, "{}", current.error_or("unknown error"));
				return {};
			}
		}
	} else {
		auto current = _client.get<std::vector<TaskData>>("tasks");
		if (current.has_value()) {
			res.append_range(current.value());
		} else {
			std::println(stderr, "{}", current.error_or("unknown error"));
			return {};
		}
	}

	for (const auto& task: res) {
		tasks[task.name].push_back(task);
	}

	for (const auto& [name, subTasks]: tasks) {
		std::println("{}", name);
		for (std::string start = " ├─ "; const auto& task: subTasks) {
		    if (task.index == subTasks.size() - 1)
			    start = " └─ ";

			std::println("{}{}.{}: {}", start, task.name, task.index, to_string(task.state));
		}
	}

	return std::nullopt;
}

std::expected<std::vector<TaskData>, std::string> Cli::getTask(const std::string& name) {
	return _client.get<std::vector<TaskData>>("task/{}", name);
}
