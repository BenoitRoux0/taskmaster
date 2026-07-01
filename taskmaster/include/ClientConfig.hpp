#ifndef TASKMASTER_CLIENT_CONFIG_HPP
# define TASKMASTER_CLIENT_CONFIG_HPP

# include <string>
# include <cstdint>

#include "tm_common.hpp"

struct ClientConfig {
	std::string getBaseUrl() const {
		std::string tmServer = getEnv("TM_SERVER", "127.0.0.1:54321");

		return "http://" + tmServer + "/";
	}
};

#endif //TASKMASTER_CLIENT_CONFIG_HPP
