#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include "ssmtp-mailer/mailer.hpp"
#include "ssmtp-mailer/queue_types.hpp"

namespace ssmtp_mailer {

/**
 * @brief Supported API providers
 */
enum class APIProvider {
    SENDGRID,
    MAILGUN,
    AMAZON_SES,
    PROTONMAIL,
    ZOHO_MAIL,
    FASTMAIL,
    POSTMARK,
    SPARKPOST,
    MAILJET,
    CUSTOM
};

/**
 * @brief API authentication methods
 */
enum class APIAuthMethod {
    API_KEY,
    OAUTH2,
    BEARER_TOKEN,
    BASIC_AUTH,
    CUSTOM_HEADERS
};

/**
 * @brief API request configuration
 */
struct APIRequestConfig {
    std::string base_url;
    std::string endpoint;
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> query_params;
    std::map<std::string, std::string> custom_headers;
    int timeout_seconds;
    bool verify_ssl;
    
    APIRequestConfig() : timeout_seconds(30), verify_ssl(true) {}
};

/**
 * @brief API authentication configuration
 */
struct APIAuthConfig {
    APIAuthMethod method;
    std::string api_key;
    std::string api_secret;
    std::string oauth2_token;
    std::string bearer_token;
    std::string username;
    std::string password;
    std::map<std::string, std::string> custom_headers;
    
    APIAuthConfig() : method(APIAuthMethod::API_KEY) {}
};

/**
 * @brief API client configuration
 */
struct APIClientConfig {
    APIProvider provider;
    APIAuthConfig auth;
    APIRequestConfig request;
    std::string sender_email;
    std::string sender_name;
    bool enable_tracking;
    std::string webhook_url;
    
    APIClientConfig() : provider(APIProvider::SENDGRID), enable_tracking(false) {}
};

/**
 * @brief API response structure
 */
struct APIResponse {
    bool success;
    int http_code;
    std::string message_id;
    std::string error_message;
    std::map<std::string, std::string> headers;
    std::string raw_response;
    
    APIResponse() : success(false), http_code(0) {}
};

/**
 * @brief Abstract base class for API clients
 */
class BaseAPIClient {
public:
    virtual ~BaseAPIClient() = default;
    
    /**
     * @brief Send an email via API
     * @param email Email to send
     * @return APIResponse with operation result
     */
    virtual APIResponse sendEmail(const Email& email) = 0;
    
    /**
     * @brief Send multiple emails in batch
     * @param emails Vector of emails to send
     * @return Vector of APIResponse results
     */
    virtual std::vector<APIResponse> sendBatch(const std::vector<Email>& emails) = 0;
    
    /**
     * @brief Test API connection
     * @return true if successful, false otherwise
     */
    virtual bool testConnection() = 0;
    
    /**
     * @brief Get provider name
     * @return Provider name string
     */
    virtual std::string getProviderName() const = 0;
    
    /**
     * @brief Check if client is properly configured
     * @return true if valid, false otherwise
     */
    virtual bool isValid() const = 0;
};

/**
 * @brief SendGrid API client implementation
 */
class SendGridAPIClient : public BaseAPIClient {
public:
    explicit SendGridAPIClient(const APIClientConfig& config);
    
    APIResponse sendEmail(const Email& email) override;
    std::vector<APIResponse> sendBatch(const std::vector<Email>& emails) override;
    bool testConnection() override;
    std::string getProviderName() const override { return "SendGrid"; }
    bool isValid() const override;

private:
    APIClientConfig config_;
    std::string buildRequestBody(const Email& email);
    std::map<std::string, std::string> buildHeaders();
};

/**
 * @brief Mailgun API client implementation
 */
class MailgunAPIClient : public BaseAPIClient {
public:
    explicit MailgunAPIClient(const APIClientConfig& config);
    
    APIResponse sendEmail(const Email& email) override;
    std::vector<APIResponse> sendBatch(const std::vector<Email>& emails) override;
    bool testConnection() override;
    std::string getProviderName() const override { return "Mailgun"; }
    bool isValid() const override;

private:
    APIClientConfig config_;
    std::string buildRequestBody(const Email& email);
    std::map<std::string, std::string> buildHeaders();
};

/**
 * @brief Amazon SES API client implementation
 */
class AmazonSESAPIClient : public BaseAPIClient {
public:
    explicit AmazonSESAPIClient(const APIClientConfig& config);
    
    APIResponse sendEmail(const Email& email) override;
    std::vector<APIResponse> sendBatch(const std::vector<Email>& emails) override;
    bool testConnection() override;
    std::string getProviderName() const override { return "Amazon SES"; }
    bool isValid() const override;

private:
    APIClientConfig config_;
    std::string buildRequestBody(const Email& email);
    std::map<std::string, std::string> buildHeaders();
};

/**
 * @brief ProtonMail API client implementation
 */
class ProtonMailAPIClient : public BaseAPIClient {
public:
    explicit ProtonMailAPIClient(const APIClientConfig& config);
    
    APIResponse sendEmail(const Email& email) override;
    std::vector<APIResponse> sendBatch(const std::vector<Email>& emails) override;
    bool testConnection() override;
    std::string getProviderName() const override { return "ProtonMail"; }
    bool isValid() const override;

private:
    APIClientConfig config_;
    std::string buildRequestBody(const Email& email);
    std::map<std::string, std::string> buildHeaders();
};

/**
 * @brief Zoho Mail API client implementation
 */
class ZohoMailAPIClient : public BaseAPIClient {
public:
    explicit ZohoMailAPIClient(const APIClientConfig& config);
    
    APIResponse sendEmail(const Email& email) override;
    std::vector<APIResponse> sendBatch(const std::vector<Email>& emails) override;
    bool testConnection() override;
    std::string getProviderName() const override { return "Zoho Mail"; }
    bool isValid() const override;

private:
    APIClientConfig config_;
    std::string buildRequestBody(const Email& email);
    std::map<std::string, std::string> buildHeaders();
};

/**
 * @brief Fastmail API client implementation
 */
class FastmailAPIClient : public BaseAPIClient {
public:
    explicit FastmailAPIClient(const APIClientConfig& config);
    
    APIResponse sendEmail(const Email& email) override;
    std::vector<APIResponse> sendBatch(const std::vector<Email>& emails) override;
    bool testConnection() override;
    std::string getProviderName() const override { return "Fastmail"; }
    bool isValid() const override;

private:
    APIClientConfig config_;
    std::string buildRequestBody(const Email& email);
    std::map<std::string, std::string> buildHeaders();
};

/**
 * @brief Factory class for creating API clients
 */
class APIClientFactory {
public:
    /**
     * @brief Create API client based on provider
     * @param config Configuration for the client
     * @return Shared pointer to API client
     */
    static std::shared_ptr<BaseAPIClient> createClient(const APIClientConfig& config);
    
    /**
     * @brief Get list of supported providers
     * @return Vector of provider names
     */
    static std::vector<std::string> getSupportedProviders();
    
    /**
     * @brief Check if provider is supported
     * @param provider Provider to check
     * @return true if supported, false otherwise
     */
    static bool isProviderSupported(APIProvider provider);
};

} // namespace ssmtp_mailer
