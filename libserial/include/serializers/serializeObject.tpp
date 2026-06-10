#ifndef SERIALIZE_OBJECT_TPP
#define SERIALIZE_OBJECT_TPP

namespace stackixx {
	template <typename T>
	std::string serializeObject(const T& value, size_t indent) {
		constexpr auto members = std::meta::reflect_constant_array(
			std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current()));

		std::string out = "{\n";

		std::vector<std::string> names;

		template for (constexpr auto member : [:members:]) {
			constexpr auto memberId = std::meta::identifier_of(member);
			auto name = getMemberName<T>(memberId);
			if (std::find(names.begin(), names.end(), name) != names.end())
				throw std::exception();
			names.push_back(name);
			out += std::string("\t") * (indent + 1);
			out += serialize(name, indent + 1);
			out += ": " + serialize(value.[:member:], indent + 1) + ",\n";
		}

		out.resize(out.length() - 2);

		out += "\n" + std::string("\t") * (indent) + "}";

		return out;
	}
} // namespace stackixx

#endif // SERIALIZE_OBJECT_TPP
