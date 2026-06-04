#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <map>
#include <optional>
#include <sys/epoll.h>

#include "HttpResponse.hpp"
#include "HttpSessionSocket.hpp"
#include "ServerConf.hpp"

class Socket;

class HttpServer {
	int                                      _epollFd;
	int                                      _accept_socket = -1;
	std::optional<ServerConf>                _conf;
	std::map<int, Socket*>                   _sockets;
	epoll_event*                             _events;
	std::function<HttpResponse(HttpRequest)> _onRequest;
	std::vector<int>                         _toRemove;
	bool                                     _stop{false};

public:
	HttpServer();
	~HttpServer();

	void loadConf(ServerConf conf);

	void run();

	void registerSession(int sock);

	void sendResponse(int socket, const HttpResponse& response);
	void endSending(int socket);
	void handleRequest(int socket, const HttpRequest& http_request);

	void onRequest(std::function<HttpResponse(HttpRequest)> callback);
	void stop();
	void remove(int socket);
};
#endif // HTTP_SERVER_HPP
