#include "HttpResponse.hpp"

#include <format>
#include <utility>

HttpResponse::HttpResponse(std::string  protocol, std::string  version, std::string  status,
                           std::string  reason,
                           const std::map<std::string, std::string>& headers,
                           std::string  body): _protocol(std::move(protocol)),
                                                     _version(std::move(version)), _status(std::move(status)), _reason(std::move(reason)),
                                                     _headers(headers), _body(std::move(body)) {
	_headers["Content-Length"] = std::to_string(_body.length());
	_headers["Connection"] = "keep-alive";
}

HttpResponse::HttpResponse(std::string content): _body(std::move(content)) {
	_headers["Content-Length"] = std::to_string(_body.length());
	_headers["Connection"] = "keep-alive";
}

HttpResponse::HttpResponse(std::string status, std::string content): _status(std::move(status)), _body(std::move(content)) {
	_headers["Content-Length"] = std::to_string(_body.length());
	_headers["Connection"] = "keep-alive";
}

const std::string& HttpResponse::getProtocol() {
	return _protocol;
}

const std::string& HttpResponse::getVersion() {
	return _version;
}

const std::string& HttpResponse::getStatus() {
	return _status;
}

const std::string& HttpResponse::getReason() {
	return _reason;
}

const std::string& HttpResponse::getHeader(const std::string& field) {
	return _headers.at(field);
}

const std::map<std::string, std::string>& HttpResponse::getHeaders() {
	return _headers;
}

const std::string& HttpResponse::getBody() {
	return _body;
}

std::string HttpResponse::toString() const {
	auto out =  std::format("{}/{} {} {}\r\n", _protocol, _version, _status, _reason);

	for (auto [field, value]: _headers) {
		out += std::format("{}: {}\r\n", field, value);
	}

	out += "\r\n";

	out += _body;

	return out;
}
