#include "ssmtp-mailer/http_client.hpp"
#include <curl/curl.h>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace ssmtp_mailer {

// Forward declaration of implementation class
class CURLHTTPClient::Impl {
public:
    CURL* curl_handle;
    std::string user_agent;
    int timeout_seconds;
    bool verify_ssl;
    std::string proxy_url;
    std::string proxy_username;
    std::string proxy_password;
    
    Impl() : curl_handle(nullptr), timeout_seconds(30), verify_ssl(true) {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl_handle = curl_easy_init();
        if (curl_handle) {
            // Set default options
            curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl_handle, CURLOPT_MAXREDIRS, 5L);
            curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, timeout_seconds);
            curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, verify_ssl ? 1L : 0L);
            curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, verify_ssl ? 2L : 0L);
            curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "ssmtp-mailer/0.2.0");
        }
    }
    
    ~Impl() {
        if (curl_handle) {
            curl_easy_cleanup(curl_handle);
        }
        curl_global_cleanup();
    }
    
    void resetHandle() {
        if (curl_handle) {
            curl_easy_reset(curl_handle);
            // Re-apply default options
            curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl_handle, CURLOPT_MAXREDIRS, 5L);
            curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, timeout_seconds);
            curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, verify_ssl ? 1L : 0L);
            curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, verify_ssl ? 2L : 0L);
            if (!user_agent.empty()) {
                curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, user_agent.c_str());
            }
            if (!proxy_url.empty()) {
                curl_easy_setopt(curl_handle, CURLOPT_PROXY, proxy_url.c_str());
                if (!proxy_username.empty()) {
                    std::string proxy_auth = proxy_username + ":" + proxy_password;
                    curl_easy_setopt(curl_handle, CURLOPT_PROXYUSERPWD, proxy_auth.c_str());
                }
            }
        }
    }
};

// Static callback functions for libcurl
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

static size_t HeaderCallback(char* buffer, size_t size, size_t nitems, std::map<std::string, std::string>* headers) {
    std::string header_line(buffer, size * nitems);
    size_t colon_pos = header_line.find(':');
    if (colon_pos != std::string::npos) {
        std::string key = header_line.substr(0, colon_pos);
        std::string value = header_line.substr(colon_pos + 1);
        // Remove trailing \r\n
        if (!value.empty() && value.back() == '\n') {
            value.pop_back();
        }
        if (!value.empty() && value.back() == '\r') {
            value.pop_back();
        }
        // Trim whitespace
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        (*headers)[key] = value;
    }
    return size * nitems;
}

static int ProgressCallback(void* clientp, double dltotal, double dlnow, double ultotal, double ulnow) {
    (void)dltotal; // Suppress unused parameter warning
    (void)dlnow;   // Suppress unused parameter warning
    auto* callback = static_cast<std::function<void(size_t, size_t)>*>(clientp);
    if (callback && *callback) {
        (*callback)(static_cast<size_t>(ulnow), static_cast<size_t>(ultotal));
    }
    return 0;
}

// CURLHTTPClient implementation
CURLHTTPClient::CURLHTTPClient() : pimpl_(std::make_unique<Impl>()) {}

CURLHTTPClient::~CURLHTTPClient() = default;

HTTPResponse CURLHTTPClient::sendRequest(const HTTPRequest& request) {
    return sendRequest(request, nullptr);
}

HTTPResponse CURLHTTPClient::sendRequest(const HTTPRequest& request, 
                                       std::function<void(size_t, size_t)> progress_callback) {
    HTTPResponse response;
    
    if (!pimpl_->curl_handle) {
        response.error_message = "CURL handle not initialized";
        return response;
    }
    
    pimpl_->resetHandle();
    
    // Build URL with query parameters
    std::string url = request.url;
    if (!request.query_params.empty()) {
        url += "?";
        bool first = true;
        for (const auto& param : request.query_params) {
            if (!first) url += "&";
            url += param.first + "=" + param.second;
            first = false;
        }
    }
    
    curl_easy_setopt(pimpl_->curl_handle, CURLOPT_URL, url.c_str());
    
    // Set HTTP method
    switch (request.method) {
        case HTTPMethod::GET:
            curl_easy_setopt(pimpl_->curl_handle, CURLOPT_HTTPGET, 1L);
            break;
        case HTTPMethod::POST:
            curl_easy_setopt(pimpl_->curl_handle, CURLOPT_POST, 1L);
            if (!request.body.empty()) {
                curl_easy_setopt(pimpl_->curl_handle, CURLOPT_POSTFIELDS, request.body.c_str());
            }
            break;
        case HTTPMethod::PUT:
            curl_easy_setopt(pimpl_->curl_handle, CURLOPT_CUSTOMREQUEST, "PUT");
            if (!request.body.empty()) {
                curl_easy_setopt(pimpl_->curl_handle, CURLOPT_POSTFIELDS, request.body.c_str());
            }
            break;
        case HTTPMethod::DELETE:
            curl_easy_setopt(pimpl_->curl_handle, CURLOPT_CUSTOMREQUEST, "DELETE");
            break;
        case HTTPMethod::PATCH:
            curl_easy_setopt(pimpl_->curl_handle, CURLOPT_CUSTOMREQUEST, "PATCH");
            if (!request.body.empty()) {
                curl_easy_setopt(pimpl_->curl_handle, CURLOPT_POSTFIELDS, request.body.c_str());
            }
            break;
    }
    
    // Set headers
    struct curl_slist* headers = nullptr;
    for (const auto& header : request.headers) {
        std::string header_line = header.first + ": " + header.second;
        headers = curl_slist_append(headers, header_line.c_str());
    }
    if (headers) {
        curl_easy_setopt(pimpl_->curl_handle, CURLOPT_HTTPHEADER, headers);
    }
    
    // Set request-specific options
    if (request.timeout_seconds > 0) {
        curl_easy_setopt(pimpl_->curl_handle, CURLOPT_TIMEOUT, request.timeout_seconds);
    }
    curl_easy_setopt(pimpl_->curl_handle, CURLOPT_SSL_VERIFYPEER, request.verify_ssl ? 1L : 0L);
    curl_easy_setopt(pimpl_->curl_handle, CURLOPT_SSL_VERIFYHOST, request.verify_ssl ? 2L : 0L);
    curl_easy_setopt(pimpl_->curl_handle, CURLOPT_FOLLOWLOCATION, request.follow_redirects ? 1L : 0L);
    
    // Set callbacks
    curl_easy_setopt(pimpl_->curl_handle, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(pimpl_->curl_handle, CURLOPT_WRITEDATA, &response.body);
    curl_easy_setopt(pimpl_->curl_handle, CURLOPT_HEADERFUNCTION, HeaderCallback);
    curl_easy_setopt(pimpl_->curl_handle, CURLOPT_HEADERDATA, &response.headers);
    
    // Set progress callback if provided
    if (progress_callback) {
        curl_easy_setopt(pimpl_->curl_handle, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(pimpl_->curl_handle, CURLOPT_XFERINFOFUNCTION, ProgressCallback);
        curl_easy_setopt(pimpl_->curl_handle, CURLOPT_XFERINFODATA, &progress_callback);
    }
    
    // Perform request
    CURLcode res = curl_easy_perform(pimpl_->curl_handle);
    
    // Clean up headers
    if (headers) {
        curl_slist_free_all(headers);
    }
    
    if (res != CURLE_OK) {
        response.error_message = curl_easy_strerror(res);
        return response;
    }
    
    // Get response code
    long response_code;
    curl_easy_getinfo(pimpl_->curl_handle, CURLINFO_RESPONSE_CODE, &response_code);
    response.status_code = static_cast<int>(response_code);
    response.success = (response.status_code >= 200 && response.status_code < 300);
    
    return response;
}

void CURLHTTPClient::setTimeout(int timeout_seconds) {
    pimpl_->timeout_seconds = timeout_seconds;
}

void CURLHTTPClient::setSSLVerification(bool verify_ssl) {
    pimpl_->verify_ssl = verify_ssl;
}

void CURLHTTPClient::setUserAgent(const std::string& user_agent) {
    pimpl_->user_agent = user_agent;
}

void CURLHTTPClient::setProxy(const std::string& proxy_url, 
                             const std::string& username, 
                             const std::string& password) {
    pimpl_->proxy_url = proxy_url;
    pimpl_->proxy_username = username;
    pimpl_->proxy_password = password;
}

// HTTPClientFactory implementation
std::shared_ptr<HTTPClient> HTTPClientFactory::createClient() {
    return std::make_shared<CURLHTTPClient>();
}

std::shared_ptr<HTTPClient> HTTPClientFactory::createClient(const std::string& backend) {
    if (backend == "curl" || backend == "libcurl") {
        return std::make_shared<CURLHTTPClient>();
    }
    // For now, only curl is supported
    return std::make_shared<CURLHTTPClient>();
}

std::vector<std::string> HTTPClientFactory::getAvailableBackends() {
    return {"curl"};
}

} // namespace ssmtp_mailer
