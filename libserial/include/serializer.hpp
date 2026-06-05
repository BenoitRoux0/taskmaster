#ifndef SERIALZER_HPP
#define SERIALZER_HPP

#include <map>
#include <meta>
#include <print>
#include <ranges>
#include <string_view>
#include "common.hpp"

#include "serializers/serializeArray.hpp"
#include "serializers/serializeMap.hpp"
#include "serializers/serializeObject.hpp"

namespace stackixx {
	template <typename T>
	std::string serialize(const T& value, size_t indent = 0);

	template <typename T>
	std::string serialize(const T& value, size_t indent) {
		if constexpr (isMap<T>) {
			return serializeMap<T>(value, indent);
		} else if constexpr (isIterable<T>) {
			return serializeArray<T>(value, indent);
		} else {
			return serializeObject<T>(value, indent);
		}
	}

	template <>
	inline std::string serialize<std::string>(const std::string& value, [[maybe_unused]] size_t indent) {
		return "\"" + value + "\"";
	}

	template <>
	inline std::string serialize<std::string_view>(const std::string_view& value, [[maybe_unused]] size_t indent) {
		return "\"" + std::string(value) + "\"";
	}

	template <>
	inline std::string serialize<int>(const int& value, [[maybe_unused]] size_t indent) {
		return std::to_string(value);
	}
} // namespace stackixx

#include "serializers/serializeArray.tpp"
#include "serializers/serializeMap.tpp"
#include "serializers/serializeObject.tpp"

#endif // SERIALZER_HPP
