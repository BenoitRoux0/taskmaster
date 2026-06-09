#include "HttpListener.hpp"

#include "Server.hpp"

HttpListener::HttpListener(Server& server, int sock): Socket(server, sock) {}

void HttpListener::handleEvent(uint32_t event) {
	if (!(event & EPOLLIN))
		return;

	sockaddr_in addr = {};
	socklen_t   addr_size = sizeof addr;

	int socket = accept(_fd, reinterpret_cast<sockaddr*>(&addr), &addr_size);

	_server.registerSocket(std::make_shared<HttpSessionSocket>(_server, socket));
}

void HttpListener::send([[maybe_unused]] const std::string& string) { }

bool HttpListener::keepAlive() {
	return true;
}
