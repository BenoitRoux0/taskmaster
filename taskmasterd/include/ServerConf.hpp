#ifndef SERVER_CONF_HPP
#define SERVER_CONF_HPP

#include <toml.hpp>

struct ServerConf {
	uint16_t port;
};

TOML11_DEFINE_CONVERSION_NON_INTRUSIVE(ServerConf, port)
#endif // SERVER_CONF_HPP
