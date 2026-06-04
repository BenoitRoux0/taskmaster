#ifndef COMMON_HPP
#define COMMON_HPP

#include <meta>

inline std::string operator*(std::string s, size_t n) {
	std::string out = "";

	while (n > 0) {
		out += s;
		--n;
	}

	return out;
}

namespace stackixx {
	struct Serialized {
		const char* name;
	};

	template <size_t N>
	constexpr auto serialized(const char (&name)[N]) -> Serialized {
		return Serialized{.name = std::define_static_string(name)};
	}
	//
	// constexpr auto serialized() -> Serialized { return Serialized{""}; }

	template <typename T>
	concept isIterableImpl = requires(T& t) {
		begin(t) != end(t);
		++std::declval<decltype(begin(t))&>();
		*begin(t);
	};

	template <typename T>
	concept isIterable = isIterableImpl<T>;

	template <typename T>
	concept isMapImpl = requires(T& t) {
		begin(t) != end(t);
		++std::declval<decltype(begin(t))&>();
		*begin(t);
		begin(t)->first;
	};

	template <typename T>
	concept isMap = isMapImpl<T>;

	template <typename T>
	auto getMemberName(std::string_view memberId) {
		constexpr auto members = std::meta::reflect_constant_array(
			std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::unchecked()));

		template for (constexpr auto member : [:members:]) {
			if (std::meta::identifier_of(member) == memberId) {
				constexpr auto annotations = std::meta::reflect_constant_array(
					std::meta::annotations_of_with_type(member, ^^stackixx::Serialized));

				template for (constexpr std::meta::info annotation : [:annotations:]) {
					return std::string(std::meta::extract<Serialized>(annotation).name);
				}
			}
		}

		return std::string(memberId);
	}
} // namespace stackixx

#endif // COMMON_HPP
