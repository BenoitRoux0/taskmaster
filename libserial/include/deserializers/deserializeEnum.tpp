#ifndef DESERIALIZE_ENUM_TPP
#define DESERIALIZE_ENUM_TPP

namespace stackixx {
	template <typename T>
	T deserializeEnum(const char* value, char** end) {
		std::string raw;

		if (!value)
			return T();

		const char* ptr = value;

		skipWhitespaces(ptr);

		for (; *ptr != '\0' && !isspace(*ptr) && *(ptr - 1) != '\\'; ++ptr) {
			raw.push_back(*ptr);
		}

		if (end)
			*end = const_cast<char*>(ptr);

		constexpr auto values = std::meta::reflect_constant_array(std::meta::enumerators_of(^^T));

		template for (constexpr auto enumerator: [:values:]) {
			if (std::meta::identifier_of(enumerator) == raw)
				return [:enumerator:];
		}

		return T(-1);
	}
} // namespace stackixx

#endif // DESERIALIZE_ENUM_TPP
