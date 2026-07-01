#ifndef SERIALIZE_ENUM_TPP
#define SERIALIZE_ENUM_TPP

namespace stackixx {
	template <typename T>
	std::string serializeEnum(const T& value, [[maybe_unused]] size_t indent) {
		constexpr auto values = std::meta::reflect_constant_array(std::meta::enumerators_of(^^T));

		template for (constexpr auto enumerator: [:values:]) {
			if ([:enumerator:] == value)
				return std::string(std::meta::identifier_of(enumerator));
		}

		throw std::runtime_error("invalid ");
	}
}

#endif // SERIALIZE_ENUM_TPP
