#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <map>
#include <optional>
#include <sys/epoll.h>

#include "HttpResponse.hpp"
#include "HttpSessionSocket.hpp"
#include "ServerConf.hpp"
#include <sys/signalfd.h>

class Socket;

class Server {
	int                                            _epollFd;
	int                                            _accept_socket = -1;
	std::optional<ServerConf>                      _conf;
	std::map<int, std::shared_ptr<Socket>>         _sockets;
	std::function<HttpResponse(HttpRequest)>       _onHttpRequest;
	std::function<void(signalfd_siginfo)>          _onChildRequest;
	std::function<void(std::chrono::milliseconds)> _onWakeUp;
	std::vector<int>                               _toRemove;
	bool                                           _stop{false};

public:
	Server();
	~Server();

	void loadConf(ServerConf conf);

	void run();

	void registerSocket(std::shared_ptr<Socket> sock);

	void sendResponse(int socket, const HttpResponse& response);
	void endSending(int socket);
	void handleHttpRequest(int socket, const HttpRequest& http_request);
	void handleSignalRequest(const signalfd_siginfo& sig_request);

	void onHttpRequest(std::function<HttpResponse(HttpRequest)> callback);
	void onChildRequest(std::function<void(signalfd_siginfo)> callback);
	void onWakeUp(std::function<void(std::chrono::milliseconds)> callback);

	void stop();
	void remove(int socket);
};
#endif // HTTP_SERVER_HPP
