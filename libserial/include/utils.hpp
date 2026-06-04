#ifndef UTILS_HPP
#define UTILS_HPP

namespace stackixx {
	std::vector<std::pair<std::string, std::string>> extractRawValues(const char* value, char** end);
	void skipWhitespaces(const char*& ptr);
} // namespace stackixx

#endif // UTILS_HPP
