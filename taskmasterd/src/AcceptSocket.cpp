#include "AcceptSocket.hpp"

#include "HttpServer.hpp"

AcceptSocket::AcceptSocket(HttpServer& server, int sock): Socket(server, sock) {}

void AcceptSocket::handleEvent(uint32_t event) {
	if (!(event & EPOLLIN))
		return;

	sockaddr_in addr = {};
	socklen_t   addr_size = sizeof addr;

	int socket = accept(_socket, reinterpret_cast<sockaddr*>(&addr), &addr_size);

	_server.registerSession(socket);
}

void AcceptSocket::send([[maybe_unused]] const std::string& string) { }

bool AcceptSocket::keepAlive() {
	return true;
}
