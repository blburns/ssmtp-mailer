#pragma once

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>

namespace ssmtp_mailer {

/**
 * @brief HTTP request method
 */
enum class HTTPMethod {
    GET,
    POST,
    PUT,
    DELETE,
    PATCH
};

/**
 * @brief HTTP request structure
 */
struct HTTPRequest {
    HTTPMethod method;
    std::string url;
    std::map<std::string, std::string> headers;
    std::string body;
    std::map<std::string, std::string> query_params;
    int timeout_seconds;
    bool verify_ssl;
    bool follow_redirects;
    
    HTTPRequest() : method(HTTPMethod::GET), timeout_seconds(30), 
                   verify_ssl(true), follow_redirects(true) {}
};

/**
 * @brief HTTP response structure
 */
struct HTTPResponse {
    int status_code;
    std::map<std::string, std::string> headers;
    std::string body;
    std::string error_message;
    bool success;
    
    HTTPResponse() : status_code(0), success(false) {}
};

/**
 * @brief HTTP client interface
 */
class HTTPClient {
public:
    virtual ~HTTPClient() = default;
    
    /**
     * @brief Send HTTP request
     * @param request HTTP request to send
     * @return HTTP response
     */
    virtual HTTPResponse sendRequest(const HTTPRequest& request) = 0;
    
    /**
     * @brief Send HTTP request with callback for progress
     * @param request HTTP request to send
     * @param progress_callback Progress callback function
     * @return HTTP response
     */
    virtual HTTPResponse sendRequest(const HTTPRequest& request, 
                                   std::function<void(size_t, size_t)> progress_callback) = 0;
    
    /**
     * @brief Set default timeout
     * @param timeout_seconds Timeout in seconds
     */
    virtual void setTimeout(int timeout_seconds) = 0;
    
    /**
     * @brief Set default SSL verification
     * @param verify_ssl Whether to verify SSL certificates
     */
    virtual void setSSLVerification(bool verify_ssl) = 0;
    
    /**
     * @brief Set default user agent
     * @param user_agent User agent string
     */
    virtual void setUserAgent(const std::string& user_agent) = 0;
    
    /**
     * @brief Set proxy configuration
     * @param proxy_url Proxy URL
     * @param username Proxy username (optional)
     * @param password Proxy password (optional)
     */
    virtual void setProxy(const std::string& proxy_url, 
                         const std::string& username = "", 
                         const std::string& password = "") = 0;
};

/**
 * @brief libcurl-based HTTP client implementation
 */
class CURLHTTPClient : public HTTPClient {
public:
    CURLHTTPClient();
    ~CURLHTTPClient() override;
    
    HTTPResponse sendRequest(const HTTPRequest& request) override;
    HTTPResponse sendRequest(const HTTPRequest& request, 
                           std::function<void(size_t, size_t)> progress_callback) override;
    
    void setTimeout(int timeout_seconds) override;
    void setSSLVerification(bool verify_ssl) override;
    void setUserAgent(const std::string& user_agent) override;
    void setProxy(const std::string& proxy_url, 
                 const std::string& username = "", 
                 const std::string& password = "") override;

private:
    class Impl;
    std::unique_ptr<Impl> pimpl_;
    
    // Disable copy constructor and assignment
    CURLHTTPClient(const CURLHTTPClient&) = delete;
    CURLHTTPClient& operator=(const CURLHTTPClient&) = delete;
};

/**
 * @brief HTTP client factory
 */
class HTTPClientFactory {
public:
    /**
     * @brief Create HTTP client
     * @return Shared pointer to HTTP client
     */
    static std::shared_ptr<HTTPClient> createClient();
    
    /**
     * @brief Create HTTP client with specific backend
     * @param backend Backend name ("curl", "winhttp", etc.)
     * @return Shared pointer to HTTP client
     */
    static std::shared_ptr<HTTPClient> createClient(const std::string& backend);
    
    /**
     * @brief Get available backends
     * @return Vector of available backend names
     */
    static std::vector<std::string> getAvailableBackends();
};

} // namespace ssmtp_mailer
