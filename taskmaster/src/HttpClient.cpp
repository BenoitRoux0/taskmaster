#include "HttpClient.hpp"

#include <curl/curl.h>
#include <print>

static size_t writeCallback(char *ptr, size_t size, size_t nmemb, void *userdata) {
	std::string* response = static_cast<std::string*>(userdata);
	response->append(ptr, size * nmemb);
	return size * nmemb;
}

HttpClient::HttpClient() {
	_command.type = commandType::NOTHING;
	_curl = nullptr;
}

HttpClient::HttpClient(const ClientConfig& config) : _config(config){
	_command.type = commandType::NOTHING;
	_curl = curl_easy_init();
	if (!_curl) {
		throw std::runtime_error("Failed to initialize CURL");
	}

	curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, writeCallback);
}

HttpClient::~HttpClient() {
	if (_curl) {
		curl_easy_cleanup(_curl);
		_curl = nullptr;
	}
}

std::string HttpClient::buildUrl(const std::string& path, std::string arg) const {
	std::string fullUrl;
	if (path == "start" || path == "stop" || path == "restart") {
		fullUrl = _config.getBaseUrl() + "task/" + arg + "/" + path;
	}
	else if (path == "reload" || path == "exit") {
		fullUrl = _config.getBaseUrl() + path;
	}
	else if (path == "tasks") {
		fullUrl = _config.getBaseUrl() + path;
	}
	else if (path == "task") {
		fullUrl = _config.getBaseUrl() + path + "/" + arg;
	}
	return fullUrl;
}

void HttpClient::resetCurl() {
	curl_easy_reset(_curl);
	curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, writeCallback);
}
