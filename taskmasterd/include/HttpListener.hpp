#ifndef ACCEPT_SOCKET_HPP
#define ACCEPT_SOCKET_HPP

#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Socket.hpp"

class HttpListener: public Socket {
public:
	HttpListener(Server& server, uint16_t port);

	void handleEvent(uint32_t event) override;

	void send(const std::string& string) override;
	bool keepAlive() override;
	[[nodiscard]] bool isReady() const;

private:
	bool  _ready;
};

#endif // ACCEPT_SOCKET_HPP
