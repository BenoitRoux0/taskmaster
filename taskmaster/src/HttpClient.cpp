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
	if (path == "start" || path == "stop") {
		fullUrl = _config.getBaseUrl() + "task/" + arg + "/" + path;
	}
	else if (path == "reload" || path == "kill") {
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

std::string HttpClient::get(const std::string& url, const std::string& arg) {
	std::string	response;
	long		httpCode;

	resetCurl();

	std::string fullUrl = buildUrl(url, arg);
	// curl_easy_setopt(_curl, CURLOPT_VERBOSE, 1L); to check connexion is maintained on server
	curl_easy_setopt(_curl, CURLOPT_URL, fullUrl.c_str());
	curl_easy_setopt(_curl, CURLOPT_HTTPGET, 1L);
	curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, writeCallback);
	curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &response);

	CURLcode res = curl_easy_perform(_curl);
	if (res != CURLE_OK) {
		throw std::runtime_error(std::string("CURL error: ") + curl_easy_strerror(res));
	}

	curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &httpCode);
	if (httpCode != 200) {
		throw std::runtime_error("HTTP request failed with code: " + std::to_string(httpCode));
	}

	return response;
}

std::string HttpClient::post(const std::string& url, const std::string& arg) {
	std::string	response;
	long		httpCode;

	resetCurl();

	curl_slist* header = nullptr;
	header = curl_slist_append(header, "Content-Type: application/json");

	std::string fullUrl = buildUrl(url, arg);
	// curl_easy_setopt(_curl, CURLOPT_VERBOSE, 1L); to check connexion is maintained on server
	curl_easy_setopt(_curl, CURLOPT_URL, fullUrl.c_str());
	curl_easy_setopt(_curl, CURLOPT_POST, 1L);
	curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, header);
	curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, arg.c_str());
	curl_easy_setopt(_curl, CURLOPT_POSTFIELDSIZE, arg.length());
	curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, writeCallback);
	curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &response);

	CURLcode res = curl_easy_perform(_curl);
	curl_slist_free_all(header);

	if (res != CURLE_OK) {
		throw std::runtime_error(curl_easy_strerror(res));
	}

	curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &httpCode);
	if (httpCode != 200) {
		throw std::runtime_error("HTTP request failed with code: " + std::to_string(httpCode));
	}

	return response;
}