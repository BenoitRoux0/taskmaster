#include "Server.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <print>
#include <ranges>
#include <utility>

#include "HttpListener.hpp"
#include "HttpResponse.hpp"
#include "HttpSessionSocket.hpp"

Server::Server() = default;


void Server::run() {
	epoll_event events[1024];

	while (!_stop) {
		auto start = std::chrono::steady_clock::now();
		const int events_count = epoll_wait(_epollFd, events, _sockets.size(), 1000);

		for (int i = 0; i < events_count; ++i) {
			int fd = events[i].data.fd;
			auto it = _sockets.find(fd);

			if (it != _sockets.end() && it->second) {
				it->second->handleEvent(events[i].events);
			} else {
				epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, nullptr);
			}
		}

		for (auto toRemove: _toRemove) {
			_sockets.erase(toRemove);
			epoll_ctl(_epollFd, EPOLL_CTL_DEL, toRemove, nullptr);
		}

		_toRemove.clear();

		auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);

		_onWakeUp(delta);
	}
}

void Server::bind(uint16_t port) {
	_epollFd = epoll_create(1024);

	auto listener = std::make_shared<HttpListener>(*this, port);

	if (!listener->isReady()) {
		_ready = false;
		return;
	}

	this->registerSocket(listener);

	std::println("server port: {}", port);
	_ready = true;
}

void Server::registerSocket(const std::shared_ptr<Socket>& sock) {
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
	auto itSocket = _sockets.find(socket);
	if (itSocket == _sockets.end() || !itSocket->second) {
		return;
	}

	itSocket->second->send(response.toString());
}

void Server::endSending(const int socket) {
	epoll_event event = { };

	event.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP;
	event.data.fd = socket;

	epoll_ctl(_epollFd, EPOLL_CTL_MOD, socket, &event);
	auto itSocket = _sockets.find(socket);
	if (itSocket == _sockets.end() || !itSocket->second) {
		return;
	}

	if (!itSocket->second->keepAlive()) {
		remove(socket);
	}

	if (_stopAfterSend) {
		stop();
	}
}

void Server::stopAfterSend() {
	_stopAfterSend = true;
}

void Server::handleHttpRequest(const int socket, const HttpRequest& http_request) {
	const HttpResponse response = _onHttpRequest(http_request);
	this->sendResponse(socket, response);
}

void Server::handleSignalRequest(const signalfd_siginfo& sig_request) const {
	_onChildRequest(sig_request);
}

void Server::onHttpRequest(std::function<HttpResponse(HttpRequest)> callback) {
	_onHttpRequest = std::move(callback);
}

void Server::onChildRequest(std::function<void(signalfd_siginfo)> callback) {
	_onChildRequest = std::move(callback);
}

void Server::onWakeUp(std::function<void(std::chrono::milliseconds)> callback) {
	_onWakeUp = std::move(callback);
}

void Server::stop() {
	_stop = true;
}

void Server::remove(const int socket) {
	_toRemove.push_back(socket);
}

void Server::clearConnections() {
	close(_epollFd);
	_sockets.clear();
}

bool Server::isReady() const {
	return _ready;
}

Server::~Server() {
	std::println("destroy http server");

	close(_epollFd);
}
