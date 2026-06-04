#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <unistd.h>
#include <cstdint>
#include <string>

class HttpServer;

class Socket {
public:
	Socket() = delete;
	Socket(HttpServer& server, const int sock);

	virtual void handleEvent(uint32_t event) = 0;
	virtual void send(const std::string& string) = 0;
	virtual bool keepAlive() = 0;

	virtual ~Socket();

protected:
	HttpServer& _server;
	int _socket;
};
#endif // SOCKET_HPP
