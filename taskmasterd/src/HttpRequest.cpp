#include "HttpRequest.hpp"

#include <sstream>
#include <utility>

HttpRequest::HttpRequest(std::string url, std::string protocol, std::string version,
                         std::string method,
                         const std::map<std::string, std::string>& headers,
                         std::string body): _rawUrl(std::move(url)), _protocol(std::move(protocol)),
                                            _version(std::move(version)), _method(std::move(method)), _headers(headers),
                                            _body(std::move(body)) {}

const std::string& HttpRequest::getRawUrl() const {
	return _rawUrl;
}

const std::vector<std::string>& HttpRequest::getUrl() const {
	if (_url.has_value())
		return *_url;

	std::vector<std::string> url = {};

	std::stringstream stream(_rawUrl);
	std::string       token;

	while (getline(stream, token, '/')) {
		if (!token.empty())
			url.push_back(token);
	}

	_url = url;

	return *_url;
}

const std::string& HttpRequest::getProtocol() const {
	return _protocol;
}

const std::string& HttpRequest::getVersion() const {
	return _version;
}

const std::string& HttpRequest::getMethod() const {
	return _method;
}

const std::string& HttpRequest::getHeader(const std::string& field) const {
	return _headers.at(field);
}

const std::map<std::string, std::string>& HttpRequest::getHeaders() const {
	return _headers;
}

const std::string& HttpRequest::getBody() const {
	return _body;
}
