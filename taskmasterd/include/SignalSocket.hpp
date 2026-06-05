#ifndef SIGNAL_SOCKET_HPP
#define SIGNAL_SOCKET_HPP
#include "Socket.hpp"

class SignalSocket: public Socket {
public:
	SignalSocket(Server& server, int sock, const sigset_t* mask);

	void handleEvent(uint32_t event) override;
	void send(const std::string& string) override;
	bool keepAlive() override;
};

#endif // SIGNAL_SOCKET_HPP
