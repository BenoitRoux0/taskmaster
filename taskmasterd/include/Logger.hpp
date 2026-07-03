#ifndef LOGGER_HPP
#define LOGGER_HPP
#include <cerrno>
#include <cstdio>
#include <format>
#include <optional>
#include <print>
#include <cstring>
#include <string>
#include <unistd.h>
#include <sys/syslog.h>

class Logger {
public:
	Logger();
	Logger(std::string head, std::FILE* stream);
	~Logger();

	template< class... Args >
	void write(std::format_string<Args...> fmt, Args&&... args);

	std::optional<std::string> checkLogFile();

private:
	std::string    _head;
	std::FILE*     _stream{stdout};
	std::FILE*     _logFile{nullptr};
	std::string    _logFilePath{"./taskmasterd.log"};

	void openLogFile();
	void writeToFile(const std::string& line);
};

template<class ... Args>
void Logger::write(std::format_string<Args...> fmt, Args&&... args) {
	char   buffer[128];

	bzero(buffer, 128);

	time_t rawtime;
	tm *   timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	strftime(buffer, 127, "[%d/%m/%Y %H:%M:%S] ", timeinfo);

	std::string message = std::format(fmt, std::forward<Args>(args)...);
	std::string line = std::format("{}{}: {}\n", buffer, _head, message);

	syslog(LOG_INFO, "%s", message.c_str());
	std::print(_stream, "{}", line);
	writeToFile(line);
}

#endif // LOGGER_HPP