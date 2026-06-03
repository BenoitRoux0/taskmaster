#ifndef TASKMASTER_HTTPCLIENT_HPP
#define TASKMASTER_HTTPCLIENT_HPP

# include <string>
# include <stdexcept>
# include <curl/curl.h>

#include "CommandParser.hpp"

class HttpClient {
	public:
		HttpClient();
		HttpClient(Command command, CURL* curl);
		~HttpClient();

		std::string get(const std::string& url);
		std::string post(const std::string& url, std::string json);

	private:
		Command _command;
		CURL*	_curl;
};


#endif //TASKMASTER_HTTPCLIENT_HPP