#include "Logger.hpp"

#include <utility>

Logger* Logger::_instance = nullptr;

Logger* Logger::getInstance(std::string head, std::FILE* stream) {
	if (_instance)
		return _instance;

	_instance = new Logger(std::move(head), stream);

	return _instance;
}

Logger::Logger(std::string head, std::FILE* stream): _head(std::move(head)), _stream(stream) {}
