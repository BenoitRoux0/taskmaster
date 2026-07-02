#include "Logger.hpp"

#include <utility>

Logger::Logger(): Logger("", stdout) {}

Logger::Logger(std::string head, std::FILE* stream): _head(std::move(head)), _stream(stream) {
    openLogFile();
}

Logger::~Logger() {
    if (_logFile)
        std::fclose(_logFile);
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
        std::println(_stream, "Warning: log file '{}' is no longer accessible, disabling file logging", _logFilePath);
        std::fclose(_logFile);
        _logFile = nullptr;
        return;
    }

    std::fputs(line.c_str(), _logFile);
    std::fflush(_logFile);

    if (std::ferror(_logFile)) {
        std::println(_stream, "Warning: failed to write to log file '{}', disabling file logging", _logFilePath);
        std::fclose(_logFile);
        _logFile = nullptr;
    }
}
