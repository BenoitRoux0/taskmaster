#include "HttpServer.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <print>
#include <ranges>
#include <utility>

#include "AcceptSocket.hpp"
#include "HttpResponse.hpp"
#include "HttpSessionSocket.hpp"

HttpServer::HttpServer(): _events(nullptr) {
	_epollFd = epoll_create(1024);
}

void HttpServer::loadConf(ServerConf conf) {
	if (_conf.has_value() && _conf->port == conf.port)
		return;

	if (_accept_socket != -1) {
		close(_accept_socket);
	}

	_accept_socket = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in sin{};

	bzero(&sin, sizeof(sockaddr_in));

	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(conf.port);

	if (bind(_accept_socket, reinterpret_cast<sockaddr*>(&sin), sizeof(sin))) {
		close(_accept_socket);
		_accept_socket = -1;
		return;
	}

	if (listen(_accept_socket, 64) == -1) {
		close(_accept_socket);
		_accept_socket = -1;
		return;
	}

	_sockets.insert(std::make_pair(_accept_socket, new AcceptSocket(*this, _accept_socket)));

	epoll_event event = { };

	event.events = EPOLLIN;
	event.data.fd = _accept_socket;

	epoll_ctl(_epollFd, EPOLL_CTL_ADD, _accept_socket, &event);

	// int opt = 1;
	// setsockopt(_accept_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof (int));
	// opt = 1;
	// setsockopt(_accept_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof (int));

	std::println("server port: {}", conf.port);
}

void HttpServer::run() {
	for (;!_stop;) {
		_events = new epoll_event[_sockets.size()];

		const int events_count = epoll_wait(_epollFd, _events, _sockets.size(), -1);

		for (int i = 0; i < events_count; ++i) {
			_sockets[_events[i].data.fd]->handleEvent(_events[i].events);
		}

		delete[] _events;

		for (auto toRemove: _toRemove) {
			delete _sockets[toRemove];
			_sockets.erase(toRemove);
			epoll_ctl(_epollFd, EPOLL_CTL_DEL, toRemove, nullptr);
		}

		_toRemove.clear();
	}
}

void HttpServer::registerSession(int sock) {
	epoll_event event = { };

	event.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP;
	event.data.fd = sock;

	epoll_ctl(_epollFd, EPOLL_CTL_ADD, sock, &event);

	_sockets.insert(std::make_pair(sock, new HttpSessionSocket(*this, sock)));
}

void HttpServer::sendResponse(const int socket, const HttpResponse& response) {
	epoll_event event = { };

	event.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP | EPOLLOUT;
	event.data.fd = socket;

	epoll_ctl(_epollFd, EPOLL_CTL_MOD, socket, &event);

	_sockets[socket]->send(response.toString());
}

void HttpServer::endSending(const int socket) {
	epoll_event event = { };

	event.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP;
	event.data.fd = socket;

	epoll_ctl(_epollFd, EPOLL_CTL_MOD, socket, &event);

	if (!_sockets[socket]->keepAlive()) {
		remove(socket);
	}
}

void HttpServer::handleRequest(const int socket, const HttpRequest& http_request) {
	const HttpResponse response = _onRequest(http_request);
	this->sendResponse(socket, response);
}

void HttpServer::onRequest(std::function<HttpResponse(HttpRequest)> callback) {
	_onRequest = std::move(callback);
}

void HttpServer::stop() {
	_stop = true;
}

void HttpServer::remove(const int socket) {
	_toRemove.push_back(socket);
}

HttpServer::~HttpServer() {
	std::println("destroy http server");

	for (auto* socket: _sockets | std::views::values) {
		delete socket;
	}

	close(_epollFd);
}
