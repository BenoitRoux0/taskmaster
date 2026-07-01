#ifndef TM_COMMON_HPP
#define TM_COMMON_HPP

#include <optional>
#include <string>

std::optional<std::string> getEnv(const std::string& name);

inline std::string getEnv(const std::string& name, const std::string& defaultValue) {
	return getEnv(name).value_or(defaultValue);
}

#endif // TM_COMMON_HPP
