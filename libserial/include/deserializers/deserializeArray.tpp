#ifndef DESERIALIZE_ARRAY_TPP
#define DESERIALIZE_ARRAY_TPP

namespace stackixx {
	template <isIterable T>
	T deserializeArray(const char* value, char** end) {
		if (!value)
			return {};

		const char* ptr = value;

		skipWhitespaces(ptr);

		if (*ptr != '[')
			throw std::exception();

		++ptr;

		T out;

		while (*ptr != '\0' && *ptr != ']') {
			skipWhitespaces(ptr);
			char* new_ptr;
			out.push_back(deserialize<typename T::value_type>(ptr, &new_ptr));
			ptr = new_ptr;
			skipWhitespaces(ptr);
			if (*ptr == ']')
				continue;
			if (*ptr != ',')
				throw std::exception();
			++ptr;
		}

		if (*ptr == ']')
			++ptr;

		if (end)
			*end = const_cast<char*>(ptr);

		return out;
	}
} // namespace stackixx

#endif // DESERIALIZE_ARRAY_TPP
