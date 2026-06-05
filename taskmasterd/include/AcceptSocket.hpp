#ifndef ACCEPT_SOCKET_HPP
#define ACCEPT_SOCKET_HPP

#include <string>
#include <netinet/in.h>

#include "Socket.hpp"

class AcceptSocket: public Socket {
public:
	AcceptSocket(Server& server, int sock);

	void handleEvent(uint32_t event) override;

	void send(const std::string& string) override;
	bool keepAlive() override;
};

#endif // ACCEPT_SOCKET_HPP
