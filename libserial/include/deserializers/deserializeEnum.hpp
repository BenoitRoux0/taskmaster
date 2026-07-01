#ifndef DESERIALIZE_ENUM_HPP
#define DESERIALIZE_ENUM_HPP

namespace stackixx {
	template <typename T>
	T deserializeEnum(const char* value, char** end);
} // namespace stackixx

#endif // DESERIALIZE_ENUM_HPP
