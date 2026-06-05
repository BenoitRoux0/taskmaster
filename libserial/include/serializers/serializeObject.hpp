#ifndef SERIALIZE_OBJECT_HPP
#define SERIALIZE_OBJECT_HPP

#include <string>

namespace stackixx {
	template <typename T>
	std::string serializeObject(const T& value, size_t indent);
}

#endif // SERIALIZE_OBJECT_HPP