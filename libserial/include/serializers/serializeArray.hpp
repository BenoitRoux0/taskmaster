#ifndef SERIALIZE_ARRAY_HPP
#define SERIALIZE_ARRAY_HPP

#include <string>

namespace stackixx {
	template <typename T>
	std::string serializeArray(const T& value, size_t indent);
}

#endif // SERIALIZE_ARRAY_HPP

