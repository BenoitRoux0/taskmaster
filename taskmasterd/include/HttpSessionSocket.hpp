#ifndef HTTP_SESSION_SOCKET_HPP
#define HTTP_SESSION_SOCKET_HPP

#include "HttpRequestBuilder.hpp"
#include "Socket.hpp"
#include "llhttp.h"

class HttpSessionSocket: public Socket {
	llhttp_settings_t settings{};
	llhttp_t          parser{};
	std::string       _sendBuffer;

	void _handleEpollIn();
	void _handleEpollOut();

public:
	explicit HttpSessionSocket(HttpServer& server, int sock);

	void handleEvent(uint32_t event) override;
	void handleRequest();
	void send(const std::string& string) override;
	bool keepAlive() override;

	HttpRequestBuilder request_builder;
};

#endif // HTTP_SESSION_SOCKET_HPP
