#ifndef TM_COMMON_HPP
#define TM_COMMON_HPP

#include <optional>
#include <string>

std::optional<std::string> getEnv(const std::string& name);

inline std::string getEnv(const std::string& name, const std::string& defaultValue) {
	return getEnv(name).value_or(defaultValue);
}

inline int getEnv(const std::string& name, int defaultValue) {
	auto raw = getEnv(name);

	if (!raw.has_value())
		return defaultValue;

	try {
		return std::stoi(*raw);
	} catch (std::exception&) {
		return defaultValue;
	}
}

#endif // TM_COMMON_HPP
