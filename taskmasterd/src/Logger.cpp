#include "Logger.hpp"

#include <utility>
#include <print>
#include <syslog.h>

static const std::string name {"taskmaster"};

Logger::Logger(): Logger("", stdout) {}

Logger::Logger(std::string head, std::FILE* stream): _head(std::move(head)), _stream(stream) {
	openLogFile();
	openlog(name.c_str(), 0, LOG_DAEMON);
}

Logger::~Logger() {
	if (_logFile)
		std::fclose(_logFile);

	closelog();
}

void Logger::openLogFile() {
	_logFile = std::fopen(_logFilePath.c_str(), "w");
	if (!_logFile)
		std::println(_stream, "Warning: unable to open log file '{}': {}", _logFilePath, strerror(errno));
}

std::optional<std::string> Logger::checkLogFile() {
	if (access(_logFilePath.c_str(), F_OK) != 0) {
		if (_logFile) {
			std::fclose(_logFile);
			_logFile = nullptr;
		}

		_logFile = std::fopen(_logFilePath.c_str(), "w");
		if (_logFile)
			return std::format("Log file '{}' was missing, it has been recreated.", _logFilePath);
		return std::format("Log file '{}' is missing and could not be recreated: {}", _logFilePath, strerror(errno));
	}

	if (access(_logFilePath.c_str(), W_OK) != 0) {
		if (_logFile) {
			std::fclose(_logFile);
			_logFile = nullptr;
		}
		return std::format("Log file '{}' exists but is not accessible: {}", _logFilePath, strerror(errno));
	}

	if (!_logFile)
		_logFile = std::fopen(_logFilePath.c_str(), "a");

	return std::nullopt;
}

void Logger::writeToFile(const std::string& line) {
	if (!_logFile)
		return;

	if (access(_logFilePath.c_str(), F_OK) != 0) {
		std::fclose(_logFile);
		_logFile = nullptr;
		return;
	}

	std::print(_logFile, "{}", line);

	if (std::ferror(_logFile)) {
		std::fclose(_logFile);
		_logFile = nullptr;
	}
}
