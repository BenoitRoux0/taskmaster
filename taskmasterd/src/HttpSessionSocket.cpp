#include "HttpSessionSocket.hpp"

#include <sys/socket.h>
#include <print>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include "TaskManager.hpp"

HttpSessionSocket::HttpSessionSocket(Server& server, int sock): Socket(server, sock) {
	llhttp_settings_init(&settings);

	settings.on_message_begin = [](llhttp_t* parser) -> int {
		auto* session = static_cast<HttpSessionSocket*>(parser->data);

		session->request_builder.reset();

		return 0;
	};

	settings.on_message_complete = []([[maybe_unused]] llhttp_t* parser) {
		auto session = static_cast<HttpSessionSocket*>(parser->data);

		session->handleRequest();

		return 0;
	};

	settings.on_url = [](llhttp_t* parser, const char* at, size_t len) -> int {
		const std::string piece(at, len);
		auto              session = static_cast<HttpSessionSocket*>(parser->data);

		session->request_builder.appendUrl(piece);

		return 0;
	};

	settings.on_protocol = [](llhttp_t* parser, const char* at, size_t len) -> int {
		const std::string piece(at, len);
		auto              session = static_cast<HttpSessionSocket*>(parser->data);

		session->request_builder.appendProtocol(piece);

		return 0;
	};

	settings.on_version = [](llhttp_t* parser, const char* at, size_t len) -> int {
		const std::string piece(at, len);
		auto              session = static_cast<HttpSessionSocket*>(parser->data);

		session->request_builder.appendVersion(piece);

		return 0;
	};

	settings.on_version = [](llhttp_t* parser, const char* at, size_t len) -> int {
		const std::string piece(at, len);
		auto              session = static_cast<HttpSessionSocket*>(parser->data);

		session->request_builder.appendVersion(piece);

		return 0;
	};

	settings.on_method = [](llhttp_t* parser, const char* at, size_t len) -> int {
		const std::string piece(at, len);
		auto              session = static_cast<HttpSessionSocket*>(parser->data);

		session->request_builder.appendMethod(piece);

		return 0;
	};

	settings.on_header_field = [](llhttp_t* parser, const char* at, size_t len) -> int {
		const std::string piece(at, len);
		auto              session = static_cast<HttpSessionSocket*>(parser->data);

		session->request_builder.appendHeaderField(piece);

		return 0;
	};

	settings.on_header_value = [](llhttp_t* parser, const char* at, size_t len) -> int {
		const std::string piece(at, len);
		auto              session = static_cast<HttpSessionSocket*>(parser->data);

		session->request_builder.appendHeaderValue(piece);

		return 0;
	};

	settings.on_header_value_complete = [](llhttp_t* parser) -> int {
		auto session = static_cast<HttpSessionSocket*>(parser->data);

		session->request_builder.endHeaderValue();

		return 0;
	};

	settings.on_body = [](llhttp_t* parser, const char* at, size_t len) -> int {
		const std::string piece(at, len);
		auto              session = static_cast<HttpSessionSocket*>(parser->data);

		session->request_builder.appendBody(piece);

		return 0;
	};

	llhttp_init(&parser, HTTP_REQUEST, &settings);
	parser.data = this;
}

void HttpSessionSocket::_handleEpollIn() {
	char buffer[1024];

	const int received = recv(_fd, buffer, 1024, 0);

	if (received <= 0) {
		_server.remove(_fd);
		return;
	}

	llhttp_execute(&parser, buffer, received);
}

void HttpSessionSocket::_handleEpollOut() {
	std::string toSend = _sendBuffer;
	std::string remaining;

	if (_sendBuffer.length() > 1024) {
		toSend = _sendBuffer.substr(0, 1024);
		remaining = _sendBuffer.substr(1024);
	}

	::send(_fd, toSend.c_str(), toSend.length(), 0);

	_sendBuffer = remaining;

	if (_sendBuffer.empty()) {
		_server.endSending(_fd);
	}
}

void HttpSessionSocket::handleEvent(const uint32_t event) {
	if (event & EPOLLIN) {
		_handleEpollIn();
	} else if (event & EPOLLOUT) {
		_handleEpollOut();
	} else if (event & EPOLLRDHUP || event & EPOLLHUP) {
		_server.remove(_fd);
	}
}

void HttpSessionSocket::handleRequest() {
	_server.handleHttpRequest(_fd, request_builder.getRequest());
	request_builder.reset();
}

void HttpSessionSocket::send(const std::string& string) {
	_sendBuffer += string;
}

bool HttpSessionSocket::keepAlive() {
	return true;
}

HttpSessionSocket::~HttpSessionSocket() {
	std::println("shutdown: {}", _fd);

	shutdown(_fd, SHUT_RDWR);
}
