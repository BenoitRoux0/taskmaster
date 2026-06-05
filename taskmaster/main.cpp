#include <print>

#include "include/Cli.hpp"

int main() {
	std::println("Hello from client");

	curl_global_init(CURL_GLOBAL_ALL);
	if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK) {
		std::print("Failed to initialize curl");
		return 1;
	}
	Cli cli;
	cli.run();
	curl_global_cleanup();
	return 0;
}
