#pragma once

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <chrono>

namespace ssmtp_mailer {

/**
 * @brief Webhook event types
 */
enum class WebhookEventType {
    DELIVERED,          // Email delivered successfully
    BOUNCED,            // Email bounced (hard/soft)
    DROPPED,            // Email dropped by provider
    OPENED,             // Email opened by recipient
    CLICKED,            // Link clicked in email
    UNSUBSCRIBED,       // Recipient unsubscribed
    SPAM_REPORT,        // Marked as spam
    DEFERRED,           // Delivery deferred
    PROCESSED,          // Email processed by provider
    UNKNOWN             // Unknown event type
};

/**
 * @brief Webhook event data
 */
struct WebhookEvent {
    WebhookEventType type;
    std::string message_id;
    std::string recipient;
    std::string sender;
    std::string subject;
    std::string timestamp;
    std::string provider;
    std::string reason;  // For bounces, drops, etc.
    std::map<std::string, std::string> metadata;
    
    WebhookEvent() : type(WebhookEventType::UNKNOWN) {}
};

/**
 * @brief Webhook configuration
 */
struct WebhookConfig {
    std::string endpoint;
    std::string secret;
    std::string method;  // GET, POST, PUT
    int timeout_seconds;
    bool verify_ssl;
    std::map<std::string, std::string> headers;
    std::vector<WebhookEventType> events;  // Events to forward
    
    WebhookConfig() : timeout_seconds(30), verify_ssl(true), method("POST") {}
};

/**
 * @brief Webhook handler interface
 */
class WebhookHandler {
public:
    virtual ~WebhookHandler() = default;
    
    /**
     * @brief Process webhook payload
     * @param payload Raw webhook payload
     * @param headers HTTP headers
     * @param provider Provider name
     * @return Vector of parsed events
     */
    virtual std::vector<WebhookEvent> processPayload(const std::string& payload, 
                                                   const std::map<std::string, std::string>& headers,
                                                   const std::string& provider) = 0;
    
    /**
     * @brief Verify webhook signature
     * @param payload Raw payload
     * @param signature Signature to verify
     * @param secret Secret key
     * @return true if signature is valid
     */
    virtual bool verifySignature(const std::string& payload, 
                               const std::string& signature, 
                               const std::string& secret) = 0;
    
    /**
     * @brief Get supported providers
     * @return Vector of provider names
     */
    virtual std::vector<std::string> getSupportedProviders() const = 0;
    
    /**
     * @brief Check if provider is supported
     * @param provider Provider name
     * @return true if supported
     */
    virtual bool isProviderSupported(const std::string& provider) const = 0;
};

/**
 * @brief SendGrid webhook handler
 */
class SendGridWebhookHandler : public WebhookHandler {
public:
    std::vector<WebhookEvent> processPayload(const std::string& payload, 
                                           const std::map<std::string, std::string>& headers,
                                           const std::string& provider) override;
    
    bool verifySignature(const std::string& payload, 
                        const std::string& signature, 
                        const std::string& secret) override;
    
    std::vector<std::string> getSupportedProviders() const override;
    bool isProviderSupported(const std::string& provider) const override;

private:
    WebhookEvent parseSendGridEvent(const std::string& event_data);
    WebhookEventType mapSendGridEventType(const std::string& event_type);
};

/**
 * @brief Mailgun webhook handler
 */
class MailgunWebhookHandler : public WebhookHandler {
public:
    std::vector<WebhookEvent> processPayload(const std::string& payload, 
                                           const std::map<std::string, std::string>& headers,
                                           const std::string& provider) override;
    
    bool verifySignature(const std::string& payload, 
                        const std::string& signature, 
                        const std::string& secret) override;
    
    std::vector<std::string> getSupportedProviders() const override;
    bool isProviderSupported(const std::string& provider) const override;

private:
    WebhookEvent parseMailgunEvent(const std::map<std::string, std::string>& form_data);
    WebhookEventType mapMailgunEventType(const std::string& event_type);
};

/**
 * @brief Amazon SES webhook handler
 */
class AmazonSESWebhookHandler : public WebhookHandler {
public:
    std::vector<WebhookEvent> processPayload(const std::string& payload, 
                                           const std::map<std::string, std::string>& headers,
                                           const std::string& provider) override;
    
    bool verifySignature(const std::string& payload, 
                        const std::string& signature, 
                        const std::string& secret) override;
    
    std::vector<std::string> getSupportedProviders() const override;
    bool isProviderSupported(const std::string& provider) const override;

private:
    WebhookEvent parseSESEvent(const std::string& event_data);
    WebhookEventType mapSESEventType(const std::string& event_type);
};

/**
 * @brief Webhook handler factory
 */
class WebhookHandlerFactory {
public:
    /**
     * @brief Create webhook handler for provider
     * @param provider Provider name
     * @return Webhook handler instance
     */
    static std::shared_ptr<WebhookHandler> createHandler(const std::string& provider);
    
    /**
     * @brief Get supported providers
     * @return Vector of provider names
     */
    static std::vector<std::string> getSupportedProviders();
    
    /**
     * @brief Check if provider is supported
     * @param provider Provider name
     * @return true if supported
     */
    static bool isProviderSupported(const std::string& provider);
};

/**
 * @brief Webhook event callback function type
 */
using WebhookEventCallback = std::function<void(const WebhookEvent&)>;

/**
 * @brief Webhook event processor
 */
class WebhookEventProcessor {
public:
    /**
     * @brief Constructor
     * @param config Webhook configuration
     */
    explicit WebhookEventProcessor(const WebhookConfig& config);
    
    /**
     * @brief Register event callback
     * @param event_type Event type to listen for
     * @param callback Callback function
     */
    void registerCallback(WebhookEventType event_type, WebhookEventCallback callback);
    
    /**
     * @brief Process webhook request
     * @param payload Raw payload
     * @param headers HTTP headers
     * @param provider Provider name
     * @return true if processed successfully
     */
    bool processWebhook(const std::string& payload, 
                       const std::map<std::string, std::string>& headers,
                       const std::string& provider);
    
    /**
     * @brief Get webhook statistics
     * @return Map of statistics
     */
    std::map<std::string, int> getStatistics() const;

private:
    WebhookConfig config_;
    std::map<WebhookEventType, std::vector<WebhookEventCallback>> callbacks_;
    std::map<std::string, int> statistics_;
    mutable std::mutex mutex_;
    
    void updateStatistics(const WebhookEvent& event);
};

} // namespace ssmtp_mailer
