#include "HttpClient.hpp"

#include <curl/curl.h>

HttpClient::HttpClient() {
	_command.type = commandType::NOTHING;
	_curl = nullptr;
}

HttpClient::HttpClient(Command command, CURL* curl) {
	_command	= command;
	_curl		= curl;
}

HttpClient::~HttpClient() {
	curl_easy_cleanup(_curl);
}

static size_t writeCallback(char *ptr, size_t size, size_t nmemb, void *userdata) {
	std::string* response = static_cast<std::string*>(userdata);
	response->append(ptr, size * nmemb);
	return size * nmemb;
}

std::string HttpClient::get(const std::string& url) {
	std::string	response;

	curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, writeCallback);
	curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &response);

	CURLcode res = curl_easy_perform(_curl);
	// curl_easy_cleanup(_curl);
	if (res != CURLE_OK) {
		throw std::runtime_error(curl_easy_strerror(res));
	}

	return response;
}

std::string HttpClient::post(const std::string& url, std::string json) {
	std::string	response;
	curl_slist* header = nullptr;
	header = curl_slist_append(header, "Content-Type: application/json");

	curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, header);
	curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, json.c_str());
	curl_easy_setopt(_curl, CURLOPT_POSTFIELDSIZE, json.length());
	curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, writeCallback);
	curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &response);

	CURLcode res = curl_easy_perform(_curl);
	// curl_easy_cleanup(_curl);
	curl_slist_free_all(header);
	if (res != CURLE_OK) {
		throw std::runtime_error(curl_easy_strerror(res));
	}

	return response;
}