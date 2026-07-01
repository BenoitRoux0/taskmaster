#ifndef SERIALIZE_ENUM_HPP
#define SERIALIZE_ENUM_HPP

#include <string>

namespace stackixx {
	template <typename T>
	std::string serializeEnum(const T& value, size_t indent);
}

#endif // SERIALIZE_ENUM_HPP