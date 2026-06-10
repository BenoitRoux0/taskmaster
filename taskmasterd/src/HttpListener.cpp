#include "HttpListener.hpp"

#include "Server.hpp"

HttpListener::HttpListener(Server& server, uint16_t port): Socket(server) {
	_fd = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in sin{};

	bzero(&sin, sizeof(sockaddr_in));

	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);

	if (::bind(_fd, reinterpret_cast<sockaddr*>(&sin), sizeof(sin))) {
		close(_fd);
		_fd = -1;
		_ready = false;
		return;
	}

	if (listen(_fd, 64) == -1) {
		close(_fd);
		_fd = -1;
		_ready = false;
		return;
	}

	_ready = true;
}

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

bool HttpListener::isReady() const {
	return _ready;
}
