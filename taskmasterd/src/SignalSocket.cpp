#include "SignalSocket.hpp"

#include <sys/epoll.h>
#include <sys/signalfd.h>

#include "Server.hpp"

SignalSocket::SignalSocket(Server& server, int sock, const sigset_t* mask): Socket(server, signalfd(sock, mask, 0)) {}

void SignalSocket::handleEvent(uint32_t event) {
	if (event & EPOLLIN) {
		signalfd_siginfo siginfo;

		read(_fd, &siginfo, sizeof(signalfd_siginfo));

		_server.handleSignalRequest(siginfo);
	}
}

void SignalSocket::send([[maybe_unused]] const std::string& string) {
	return;
}

bool SignalSocket::keepAlive() {
	return true;
}
