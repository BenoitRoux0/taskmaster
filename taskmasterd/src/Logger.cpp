#include "Logger.hpp"

#include <utility>

Logger::Logger(std::string head, std::FILE* stream): _head(std::move(head)), _stream(stream) {}

