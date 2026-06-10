#include "Socket.hpp"

#include <print>

Socket::Socket(Server& server): _server(server) {}

Socket::Socket(Server& server, const int sock): _server(server), _fd(sock) {}

int Socket::getFd() const {
	return _fd;
}

Socket::~Socket() {
	std::println("close: {}", _fd);
	close(_fd);
}
