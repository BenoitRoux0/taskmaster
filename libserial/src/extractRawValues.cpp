#include <string>
#include <vector>
#include "deserializer.hpp"

namespace stackixx {
	static std::string getRawValue(const char* value, char** end);

	std::vector<std::pair<std::string, std::string>> extractRawValues(const char* value, [[maybe_unused]] char** end) {
		const char* ptr = value;

		skipWhitespaces(ptr);

		if (*ptr != '{')
			throw std::exception();

		++ptr;

		std::vector<std::pair<std::string, std::string>> raw_values;

		char* new_ptr;

		while (*ptr != '\0' && *ptr != '}') {
			skipWhitespaces(ptr);
			auto name = deserialize<std::string>(ptr, &new_ptr);
			ptr = new_ptr;
			skipWhitespaces(ptr);
			if (*ptr != ':')
				throw std::exception();
			++ptr;
			skipWhitespaces(ptr);
			auto mapped = getRawValue(ptr, &new_ptr);
			raw_values.emplace_back(std::string(name), mapped);
			ptr = new_ptr;
			skipWhitespaces(ptr);
			if (*ptr == '}')
				continue;
			if (*ptr != ',')
				throw std::exception();
			++ptr;
		}

		return raw_values;
	}

	static std::string getRawValue(const char* value, char** end) {
		size_t scopes = 0;

		const char* ptr = value;

		while (*ptr != '\0') {
			if (*ptr == '{' || *ptr == '[')
				++scopes;
			if (*ptr == '}' || *ptr == ']') {
				if (scopes == 0) {
					break;
				}
				--scopes;
			}
			if (*ptr == ',' && scopes == 0) {
				break;
			}
			++ptr;
		}

		std::string out = std::string(value).substr(0, ptr - value);

		if (end)
			*end = const_cast<char*>(ptr);

		return out;
	}


} // namespace stackixx
