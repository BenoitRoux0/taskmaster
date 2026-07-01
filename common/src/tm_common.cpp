#include "tm_common.hpp"

std::optional<std::string> getEnv(const std::string& name) {
	const char* value = getenv(name.c_str());

	if (value == nullptr)
		return {};

	return value;
}

