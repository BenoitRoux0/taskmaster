#ifndef TASKMASTER_HTTPCLIENT_HPP
#define TASKMASTER_HTTPCLIENT_HPP

# include <string>
# include <stdexcept>
# include <curl/curl.h>

# include "CommandParser.hpp"
# include "ClientConfig.hpp"

class HttpClient {
	public:

		HttpClient();
		HttpClient(const ClientConfig& config);
		~HttpClient();

		std::string get(const std::string& url);
		std::string post(const std::string& url, const std::string& json);

	private:
		ClientConfig _config;
		Command _command;
		CURL*	_curl;

	std::string buildUrl(const std::string& path) const;
	void		resetCurl();
};


#endif //TASKMASTER_HTTPCLIENT_HPP