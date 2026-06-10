#ifndef LOGGER_HPP
#define LOGGER_HPP
#include <format>
#include <print>
#include <string.h>
#include <string>

class Logger {
public:
	Logger() = default;
	Logger(std::string head, std::FILE* stream);

	template< class... Args >
	void write(std::format_string<Args...> fmt, Args&&... args);


private:
	std::string    _head;
	std::FILE*     _stream{stdout};
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

	std::print(_stream, "{}{}: ", buffer, _head);
	std::println(_stream, fmt, args...);
}

#endif // LOGGER_HPP
