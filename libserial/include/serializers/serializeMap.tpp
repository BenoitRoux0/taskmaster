#ifndef SERIALIZE_MAP_TPP
#define SERIALIZE_MAP_TPP

namespace stackixx {
	template <typename T>
	std::string serializeMap(const T& value, size_t indent) {
		if (value.empty()) {
			return "{ }";
		}

		std::string out = "{\n";

		for (typename T::const_iterator it = value.begin(); it != value.end(); ++it) {
			out += std::string("\t") * (indent + 1) + serialize(it->first) + ": " + serialize(it->second);
			out += ",\n";
		}

		out.resize(out.length() - 2);

		out += "\n" + std::string("\t") * (indent) + "}";

		return out;
	}
} // namespace stackixx

#endif // SERIALIZE_MAP_TPP
