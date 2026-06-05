#include "AcceptSocket.hpp"

#include "Server.hpp"

AcceptSocket::AcceptSocket(Server& server, int sock): Socket(server, sock) {}

void AcceptSocket::handleEvent(uint32_t event) {
	if (!(event & EPOLLIN))
		return;

	sockaddr_in addr = {};
	socklen_t   addr_size = sizeof addr;

	int socket = accept(_fd, reinterpret_cast<sockaddr*>(&addr), &addr_size);

	_server.registerSocket(std::make_shared<HttpSessionSocket>(_server, socket));
}

void AcceptSocket::send([[maybe_unused]] const std::string& string) { }

bool AcceptSocket::keepAlive() {
	return true;
}
