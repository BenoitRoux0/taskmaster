#ifndef TASKMASTER_HTTPCLIENT_HPP
#define TASKMASTER_HTTPCLIENT_HPP

#include <optional>
# include <string>
# include <stdexcept>
# include <curl/curl.h>

#include "CommandParser.hpp"

class HttpClient {
	public:
		HttpClient();
		HttpClient(const Command& command, CURL* curl);
		~HttpClient();

		std::string get(const std::string& url);
		std::string post(const std::string& url, const std::string& json);

	private:
		Command _command;
		CURL*	_curl;
};


#endif //TASKMASTER_HTTPCLIENT_HPP