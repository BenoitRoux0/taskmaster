#include "SignalSocket.hpp"

#include <sys/epoll.h>
#include <sys/signalfd.h>

#include "Server.hpp"

SignalSocket::SignalSocket(Server& server, int sock, const sigset_t* mask): Socket(server, signalfd(sock, mask, SFD_NONBLOCK)) {}

void SignalSocket::handleEvent(uint32_t event) {
	if (event & EPOLLIN) {
		signalfd_siginfo siginfo;
		ssize_t ret;

		do {
			ret = read(_fd, &siginfo, sizeof(signalfd_siginfo));
			if (ret > 0)
				_server.handleSignalRequest(siginfo);
		} while (ret > 0);
	}
}

void SignalSocket::send([[maybe_unused]] const std::string& string) {
	return;
}

bool SignalSocket::keepAlive() {
	return true;
}
