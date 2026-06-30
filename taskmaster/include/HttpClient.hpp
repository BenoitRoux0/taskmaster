#ifndef TASKMASTER_HTTPCLIENT_HPP
#define TASKMASTER_HTTPCLIENT_HPP

#include <expected>
#include <format>
#include <optional>
# include <string>
# include <curl/curl.h>

#include "CommandParser.hpp"
#include "ClientConfig.hpp"
#include "deserializer.hpp"

class HttpClient {
public:
	HttpClient();
	HttpClient(const ClientConfig& config);
	~HttpClient();

	// std::string get(const std::string& url, const std::string& arg);
	// std::string post(const std::string& url, const std::string& arg);

	template<typename Out, class... Args>
	std::expected<Out, std::string> get(std::format_string<Args...> urlFmt, Args&&... args);

	template<typename Out, class... Args>
	std::expected<Out, std::string> post(std::format_string<Args...> urlFmt, Args&&... args);

private:
	ClientConfig _config;
	Command      _command;
	CURL*        _curl;

	std::string buildUrl(const std::string& path, const std::string arg) const;
	void        resetCurl();
};

static size_t testWriteCallback(char *ptr, size_t size, size_t nmemb, void *userdata) {
	std::string* response = static_cast<std::string*>(userdata);
	response->append(ptr, size * nmemb);
	return size * nmemb;
}

template<typename Out, class... Args>
std::expected<Out, std::string> HttpClient::get(std::format_string<Args...> urlFmt, Args&&... args) {
	auto        url = _config.getBaseUrl() + std::format(urlFmt, args...);
	std::string response;
	long        httpCode;

	resetCurl();

	// curl_easy_setopt(_curl, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(_curl, CURLOPT_HTTPGET, 1L);
	curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, testWriteCallback);
	curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &response);

	CURLcode res = curl_easy_perform(_curl);
	if (res != CURLE_OK) {
		return std::unexpected(response);
	}

	curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &httpCode);
	if (httpCode != 200) {
		return std::unexpected(response);
	}

	return {stackixx::deserialize<Out>(response.c_str(), nullptr)};
}

template <typename Out, class ... Args>
std::expected<Out, std::string> HttpClient::post(std::format_string<Args...> urlFmt, Args&&... args) {
	auto        url = _config.getBaseUrl() + std::format(urlFmt, args...);
	std::string response;
	long        httpCode;

	resetCurl();

	// curl_easy_setopt(_curl, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(_curl, CURLOPT_HTTPPOST, 1L);
	curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, testWriteCallback);
	curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &response);

	CURLcode res = curl_easy_perform(_curl);
	if (res != CURLE_OK) {
		return std::unexpected(response);
	}

	curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &httpCode);
	if (httpCode != 200) {
		return std::unexpected(response);
	}

	return {stackixx::deserialize<Out>(response.c_str(), nullptr)};
}


#endif //TASKMASTER_HTTPCLIENT_HPP
