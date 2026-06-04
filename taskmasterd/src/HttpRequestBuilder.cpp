#include "HttpRequestBuilder.hpp"

HttpRequestBuilder::HttpRequestBuilder() {
	reset();
}

void HttpRequestBuilder::reset() {
	_url = "";
	_protocol = "";
	_version = "";
	_method = "";
	_headerValue = "";
	_headerField = "";
	_headers = {};
	_body = "";
}

void HttpRequestBuilder::appendUrl(const std::string& piece) {
	_url += piece;
}

void HttpRequestBuilder::appendProtocol(const std::string& piece) {
	_protocol += piece;
}

void HttpRequestBuilder::appendVersion(const std::string& piece) {
	_version += piece;
}

void HttpRequestBuilder::appendMethod(const std::string& piece) {
	_method += piece;
}

void HttpRequestBuilder::appendHeaderField(const std::string& piece) {
	_headerField += piece;
}

void HttpRequestBuilder::appendHeaderValue(const std::string& piece) {
	_headerValue += piece;
}

void HttpRequestBuilder::endHeaderValue() {
	_headers[_headerField] = _headerValue;
	_headerField = "";
	_headerValue = "";
}

void HttpRequestBuilder::appendBody(const std::string& piece) {
	_body += piece;
}

HttpRequest HttpRequestBuilder::getRequest() const {
	return {_url, _protocol, _version, _method, _headers, _body};
}
