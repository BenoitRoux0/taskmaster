#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <unistd.h>
#include <cstdint>
#include <string>

class Server;

class Socket {
public:
	Socket(Server& server);
	Socket(Server& server, int sock);

	virtual void handleEvent(uint32_t event) = 0;
	virtual void send(const std::string& string) = 0;
	virtual bool keepAlive() = 0;

	[[nodiscard]] int getFd() const;

	virtual ~Socket();

protected:
	Server& _server;
	int         _fd {-1};
};
#endif // SOCKET_HPP
