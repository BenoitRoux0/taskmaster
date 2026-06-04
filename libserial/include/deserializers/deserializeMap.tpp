#ifndef DESERIALIZE_MAP_TPP
#define DESERIALIZE_MAP_TPP

namespace stackixx {
	template <typename T>
	T deserializeMap(const char* value, char** end) {
		if (!value)
			return {};

		const char* ptr = value;

		skipWhitespaces(ptr);

		if (*ptr != '{')
			throw std::exception();

		++ptr;

		T out;
		char* new_ptr;

		while (*ptr != '\0' && *ptr != '}') {
			skipWhitespaces(ptr);
			auto name = deserialize<std::string>(ptr, &new_ptr);
			ptr = new_ptr;
			skipWhitespaces(ptr);
			if (*ptr != ':')
				throw std::exception();
			++ptr;
			skipWhitespaces(ptr);
			auto mapped = deserialize<typename T::mapped_type>(ptr, &new_ptr);
			out[name] = mapped;
			ptr = new_ptr;
			skipWhitespaces(ptr);
			if (*ptr == '}')
				continue;
			if (*ptr != ',')
				throw std::exception();
			++ptr;
		}

		if (*ptr == '}')
			++ptr;

		if (end)
			*end = const_cast<char*>(ptr);

		return out;
	}
} // namespace stackixx

#endif // DESERIALIZE_MAP_TPP
