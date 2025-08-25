#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "ssmtp-mailer/mailer.hpp"
#include "ssmtp-mailer/api_client.hpp"
#include "ssmtp-mailer/queue_types.hpp"

namespace ssmtp_mailer {

/**
 * @brief Email sending method
 */
enum class SendMethod {
    SMTP,      // Traditional SMTP
    API,       // REST API
    AUTO       // Automatically choose best method
};

/**
 * @brief Unified mailer configuration
 */
struct UnifiedMailerConfig {
    SendMethod default_method;
    std::string smtp_config_file;
    std::map<std::string, APIClientConfig> api_configs;
    bool enable_fallback;
    int max_retries;
    std::chrono::seconds retry_delay;
    
    UnifiedMailerConfig() : default_method(SendMethod::AUTO), enable_fallback(true), 
                           max_retries(3), retry_delay(std::chrono::seconds(5)) {}
};

/**
 * @brief Unified mailer result
 */
struct UnifiedMailerResult {
    bool success;
    SendMethod method_used;
    std::string message_id;
    std::string error_message;
    std::string provider_name;
    int retry_count;
    
    UnifiedMailerResult() : success(false), method_used(SendMethod::SMTP), retry_count(0) {}
};

/**
 * @brief Unified mailer class that supports both SMTP and API sending
 */
class UnifiedMailer {
public:
    /**
     * @brief Constructor
     * @param config Configuration for the unified mailer
     */
    explicit UnifiedMailer(const UnifiedMailerConfig& config);
    
    /**
     * @brief Destructor
     */
    ~UnifiedMailer();
    
    /**
     * @brief Send email using specified method
     * @param email Email to send
     * @param method Sending method to use
     * @return UnifiedMailerResult with operation status
     */
    UnifiedMailerResult sendEmail(const Email& email, SendMethod method = SendMethod::AUTO);
    
    /**
     * @brief Send email via SMTP
     * @param email Email to send
     * @return UnifiedMailerResult with operation status
     */
    UnifiedMailerResult sendViaSMTP(const Email& email);
    
    /**
     * @brief Send email via API
     * @param email Email to send
     * @param provider API provider to use
     * @return UnifiedMailerResult with operation status
     */
    UnifiedMailerResult sendViaAPI(const Email& email, const std::string& provider = "");
    
    /**
     * @brief Send email with automatic method selection
     * @param email Email to send
     * @return UnifiedMailerResult with operation status
     */
    UnifiedMailerResult sendAuto(const Email& email);
    
    /**
     * @brief Send multiple emails in batch
     * @param emails Vector of emails to send
     * @param method Sending method to use
     * @return Vector of UnifiedMailerResult
     */
    std::vector<UnifiedMailerResult> sendBatch(const std::vector<Email>& emails, 
                                              SendMethod method = SendMethod::AUTO);
    
    /**
     * @brief Test connection for specified method
     * @param method Method to test
     * @param provider Provider name (for API method)
     * @return true if successful, false otherwise
     */
    bool testConnection(SendMethod method, const std::string& provider = "");
    
    /**
     * @brief Get available API providers
     * @return Vector of provider names
     */
    std::vector<std::string> getAvailableAPIProviders() const;
    
    /**
     * @brief Check if provider is available
     * @param provider Provider name
     * @return true if available, false otherwise
     */
    bool isProviderAvailable(const std::string& provider) const;
    
    /**
     * @brief Set default sending method
     * @param method Default method to use
     */
    void setDefaultMethod(SendMethod method);
    
    /**
     * @brief Add or update API configuration
     * @param provider Provider name
     * @param config API configuration
     */
    void setAPIConfig(const std::string& provider, const APIClientConfig& config);
    
    /**
     * @brief Remove API configuration
     * @param provider Provider name
     */
    void removeAPIConfig(const std::string& provider);
    
    /**
     * @brief Get statistics
     * @return Map of statistics
     */
    std::map<std::string, size_t> getStatistics() const;

private:
    UnifiedMailerConfig config_;
    std::unique_ptr<class ConfigManager> smtp_config_;
    std::map<std::string, std::shared_ptr<BaseAPIClient>> api_clients_;
    
    // Statistics
    mutable std::map<std::string, size_t> stats_;
    mutable std::mutex stats_mutex_;
    
    // Helper methods
    void initializeSMTP();
    void initializeAPIClients();
    void updateStats(const std::string& key, bool success);
    std::string selectBestProvider(const Email& email);
    bool shouldRetry(const UnifiedMailerResult& result);
    UnifiedMailerResult retryWithFallback(const Email& email, SendMethod original_method);
};

} // namespace ssmtp_mailer
