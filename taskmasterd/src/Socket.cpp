#include "Socket.hpp"

#include <print>

Socket::Socket(HttpServer& server, const int sock): _server(server), _socket(sock) {}

Socket::~Socket() {
	std::println("close: {}", _socket);
	close(_socket);
}
