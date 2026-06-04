#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP
#include <map>
#include <string>

class HttpRequest {
	std::string                        _url;
	std::string                        _protocol;
	std::string                        _version;
	std::string                        _method;
	std::map<std::string, std::string> _headers;
	std::string                        _body;

public:
	HttpRequest(std::string url, std::string protocol, std::string version, std::string method,
	            const std::map<std::string, std::string>& headers, std::string body);

	const std::string&                        getUrl() const;
	const std::string&                        getProtocol() const;
	const std::string&                        getVersion() const;
	const std::string&                        getMethod() const;
	const std::string&                        getHeader(const std::string& field) const;
	const std::map<std::string, std::string>& getHeaders() const;
	const std::string&                        getBody() const;
};

#endif // HTTP_REQUEST_HPP
