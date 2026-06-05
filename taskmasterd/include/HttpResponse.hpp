#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP
#include <map>
#include <string>

class HttpResponse {
	std::string                        _protocol {"HTTP"};
	std::string                        _version {"1.1"};
	std::string                        _status {"200"};
	std::string                        _reason {"OK"};
	std::map<std::string, std::string> _headers {};
	std::string                        _body;

public:
	HttpResponse(std::string protocol, std::string version, std::string status,
	             std::string reason, const std::map<std::string, std::string>& headers, std::string body);

	HttpResponse(std::string content);
	HttpResponse(std::string status, std::string content);

	const std::string&                        getProtocol();
	const std::string&                        getVersion();
	const std::string&                        getStatus();
	const std::string&                        getReason();
	const std::string&                        getHeader(const std::string& field);
	const std::map<std::string, std::string>& getHeaders();
	const std::string&                        getBody();

	std::string toString() const;
};

#endif // HTTP_RESPONSE_HPP
