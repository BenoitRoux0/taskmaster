#ifndef DESERIALIZE_OBJECT_TPP
#define DESERIALIZE_OBJECT_TPP

#include <meta>
#include "utils.hpp"

namespace stackixx {
	template <typename T>
	T deserializeObject(const char* value, char** end) {
		if (!value)
			return {};

		const char* ptr = value;

		T out;
		char* new_ptr;

		auto raw_values = extractRawValues(ptr, &new_ptr);

		ptr = new_ptr;

		static constexpr auto members =
			std::define_static_array(std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current()));

		template for (constexpr auto member : members) {
			for (auto [name, rawValue] : raw_values) {
				if (name == getMemberName<T>(std::meta::identifier_of(member))) {
					out.[:member:] = deserialize<typename[:std::meta::type_of(member):]>(rawValue.c_str());
				}
			}
		}

		if (end)
			*end = const_cast<char*>(ptr);

		return out;
	}
} // namespace stackixx

#endif // DESERIALIZE_OBJECT_TPP
