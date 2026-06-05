#ifndef HTTP_REQUEST_BUILDER_HPP
#define HTTP_REQUEST_BUILDER_HPP
#include <map>
#include <string>

#include "HttpRequest.hpp"

class HttpRequestBuilder {
	std::string                        _url;
	std::string                        _protocol;
	std::string                        _version;
	std::string                        _method;
	std::string                        _headerField;
	std::string                        _headerValue;
	std::map<std::string, std::string> _headers;
	std::string                        _body;

public:
	HttpRequestBuilder();

	void reset();

	void appendUrl(const std::string& piece);
	void appendProtocol(const std::string& piece);
	void appendVersion(const std::string& piece);
	void appendMethod(const std::string& piece);
	void appendHeaderField(const std::string& piece);
	void appendHeaderValue(const std::string& piece);
	void endHeaderValue();
	void appendBody(const std::string& piece);

	HttpRequest getRequest() const;
};

#endif // HTTP_REQUEST_BUILDER_HPP
