#ifndef SERIALIZE_MAP_HPP
#define SERIALIZE_MAP_HPP

#include <string>

namespace stackixx {
	template <typename T>
	std::string serializeMap(const T& value, size_t indent);
}

#endif // SERIALIZE_MAP_HPP