#ifndef DESERIALIZE_OBJECT_HPP
#define DESERIALIZE_OBJECT_HPP

namespace stackixx {
	template <typename T>
	T deserializeObject(const char* value, char** end);
}

#endif // DESERIALIZE_OBJECT_HPP
