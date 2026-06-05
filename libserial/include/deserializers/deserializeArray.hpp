#ifndef DESERIALIZE_ARRAY_HPP
#define DESERIALIZE_ARRAY_HPP

namespace stackixx {
	template <isIterable T>
	T deserializeArray(const char* value, char** end);
} // namespace stackixx

#endif // DESERIALIZE_ARRAY_HPP
