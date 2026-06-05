#include "Server.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <print>
#include <ranges>
#include <utility>

#include "AcceptSocket.hpp"
#include "HttpResponse.hpp"
#include "HttpSessionSocket.hpp"

Server::Server(): _events(nullptr) {
	_epollFd = epoll_create(1024);
}

void Server::loadConf(ServerConf conf) {
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

	std::println("server port: {}", conf.port);
}

void Server::run() {
	for (;!_stop;) {
		_events = new epoll_event[_sockets.size()];

		const int events_count = epoll_wait(_epollFd, _events, _sockets.size(), -1);

		for (int i = 0; i < events_count; ++i) {
			_sockets[_events[i].data.fd]->handleEvent(_events[i].events);
		}

		delete[] _events;

		for (auto toRemove: _toRemove) {
			_sockets.erase(toRemove);
			epoll_ctl(_epollFd, EPOLL_CTL_DEL, toRemove, nullptr);
		}

		_toRemove.clear();
	}
}

void Server::registerSocket(std::shared_ptr<Socket> sock) {
	epoll_event event = { };

	event.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP;
	event.data.fd = sock->getFd();

	epoll_ctl(_epollFd, EPOLL_CTL_ADD, sock->getFd(), &event);

	std::println("register: {}", sock->getFd());

	_sockets.insert(std::make_pair(sock->getFd(), sock));
}

void Server::sendResponse(const int socket, const HttpResponse& response) {
	epoll_event event = { };

	event.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP | EPOLLOUT;
	event.data.fd = socket;

	epoll_ctl(_epollFd, EPOLL_CTL_MOD, socket, &event);

	_sockets[socket]->send(response.toString());
}

void Server::endSending(const int socket) {
	epoll_event event = { };

	event.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP;
	event.data.fd = socket;

	epoll_ctl(_epollFd, EPOLL_CTL_MOD, socket, &event);

	if (!_sockets[socket]->keepAlive()) {
		remove(socket);
	}
}

void Server::handleHttpRequest(const int socket, const HttpRequest& http_request) {
	const HttpResponse response = _onHttpRequest(http_request);
	this->sendResponse(socket, response);
}

void Server::handleSignalRequest(const signalfd_siginfo& sig_request) {
	_onChildRequest(sig_request);
}

void Server::onHttpRequest(std::function<HttpResponse(HttpRequest)> callback) {
	_onHttpRequest = std::move(callback);
}

void Server::onChildRequest(std::function<void(signalfd_siginfo)> callback) {
	_onChildRequest = std::move(callback);
}

void Server::stop() {
	_stop = true;
}

void Server::remove(const int socket) {
	_toRemove.push_back(socket);
}

Server::~Server() {
	std::println("destroy http server");

	close(_epollFd);
}
