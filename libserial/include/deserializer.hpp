#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

#include <cstring>
#include "common.hpp"

#include "deserializers/deserializeArray.hpp"
#include "deserializers/deserializeMap.hpp"
#include "deserializers/deserializeObject.hpp"
#include "deserializers/deserializeEnum.hpp"

#include "utils.hpp"

namespace stackixx {
	template <typename T>
	T deserialize(const char* value, char** end = nullptr);

	template <typename T>
	T deserialize(const char* value, char** end) {
		if constexpr (std::is_enum_v<T>) {
			return deserializeEnum<T>(value, end);
		} else if constexpr (isMap<T>) {
			return deserializeMap<T>(value, end);
		} else if constexpr (isIterable<T>) {
			return deserializeArray<T>(value, end);
		} else {
			return deserializeObject<T>(value, end);
		}
	}

	template <>
	inline std::string deserialize(const char* value, char** end) {
		if (!value)
			return "";

		const char* ptr = value;

		skipWhitespaces(ptr);

		if (*ptr != '"')
			throw std::exception();

		++ptr;

		std::string out = "";

		for (; *ptr != '\0' && *ptr != '"' && *(ptr - 1) != '\\'; ++ptr) {
			out.push_back(*ptr);
		}

		if (*ptr == '"')
			++ptr;

		if (end)
			*end = const_cast<char*>(ptr);

		return out;
	}

	template <>
	inline int deserialize(const char* value, char** end) {
		return strtol(value, end, 10);
	}

	template <>
	inline bool deserialize(const char* value, char** end) {
		const char* ptr = value;

		bool out = false;

		skipWhitespaces(ptr);

		if (strncmp(ptr, "true", 4) == 0) {
			out = true;
			ptr += 4;
		} else if (strncmp(ptr, "false", 5) == 0) {
			out = false;
			ptr += 5;
		} else {
			throw std::exception();
		}

		if (*ptr != 0 && !isspace(*ptr))
			throw std::exception();

		if (end)
			*end = const_cast<char*>(ptr);

		return out;
	}
} // namespace stackixx

#include "deserializers/deserializeArray.tpp"
#include "deserializers/deserializeMap.tpp"
#include "deserializers/deserializeObject.tpp"
#include "deserializers/deserializeEnum.tpp"

#endif // SERIALIZER_HPP
