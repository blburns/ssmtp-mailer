#include "simple-smtp-mailer/mailer.hpp"
#include "core/logging/logger.hpp"
#include "core/config/config_manager.hpp"
#include "core/smtp/smtp_client.hpp"
#include "core/queue/email_queue.hpp"
// #include "core/auth/auth_manager.hpp"  // TODO: Implement AuthManager or use existing auth classes
#include <memory>
#include <stdexcept>

namespace ssmtp_mailer {

// Forward declaration of implementation class
class Mailer::Impl {
public:
    Impl(const std::string& config_file);
    ~Impl() = default;
    
    SMTPResult send(const Email& email);
    SMTPResult send(const std::string& from, const std::string& to, 
                    const std::string& subject, const std::string& body);
    SMTPResult sendHtml(const std::string& from, const std::string& to, 
                        const std::string& subject, const std::string& body, 
                        const std::string& html_body);
    bool isConfigured() const;
    std::string getLastError() const;
    bool testConnection();
    
    // Queue management
    void enqueue(const Email& email, EmailPriority priority = EmailPriority::NORMAL);
    void startQueue();
    void stopQueue();
    bool isQueueRunning() const;
    size_t getQueueSize() const;
    std::vector<QueueItem> getPendingEmails() const;
    std::vector<QueueItem> getFailedEmails() const;
    
private:
    std::unique_ptr<ConfigManager> config_manager_;
    std::unique_ptr<SMTPClient> smtp_client_;
    std::unique_ptr<EmailQueue> email_queue_;
    // std::unique_ptr<AuthManager> auth_manager_;  // TODO: Implement AuthManager
    std::string last_error_;
    bool is_configured_;
    
    bool initializeConfiguration(const std::string& config_file);
    bool validateEmailPermissions(const Email& email);
    SMTPResult sendEmailDirect(const Email& email);
};

// Mailer implementation
Mailer::Mailer(const std::string& config_file) 
    : pImpl(std::make_unique<Impl>(config_file)) {
}

Mailer::~Mailer() = default;

SMTPResult Mailer::send(const Email& email) {
    return pImpl->send(email);
}

SMTPResult Mailer::send(const std::string& from, const std::string& to, 
                        const std::string& subject, const std::string& body) {
    return pImpl->send(from, to, subject, body);
}

SMTPResult Mailer::sendHtml(const std::string& from, const std::string& to, 
                            const std::string& subject, const std::string& body, 
                            const std::string& html_body) {
    return pImpl->sendHtml(from, to, subject, body, html_body);
}

bool Mailer::isConfigured() const {
    return pImpl->isConfigured();
}

std::string Mailer::getLastError() const {
    return pImpl->getLastError();
}

bool Mailer::testConnection() {
    return pImpl->testConnection();
}

// Queue management methods
void Mailer::enqueue(const Email& email, EmailPriority priority) {
    pImpl->enqueue(email, priority);
}

void Mailer::startQueue() {
    pImpl->startQueue();
}

void Mailer::stopQueue() {
    pImpl->stopQueue();
}

bool Mailer::isQueueRunning() const {
    return pImpl->isQueueRunning();
}

size_t Mailer::getQueueSize() const {
    return pImpl->getQueueSize();
}

std::vector<QueueItem> Mailer::getPendingEmails() const {
    return pImpl->getPendingEmails();
}

std::vector<QueueItem> Mailer::getFailedEmails() const {
    return pImpl->getFailedEmails();
}

// Implementation class methods
Mailer::Impl::Impl(const std::string& config_file) 
    : is_configured_(false) {
    
    Logger& logger = Logger::getInstance();
    logger.info("Initializing Mailer with config: " + (config_file.empty() ? "default" : config_file));
    
    if (!initializeConfiguration(config_file)) {
        logger.error("Failed to initialize configuration: " + last_error_);
        return;
    }
    
            try {
            smtp_client_ = std::make_unique<SMTPClient>(*config_manager_);
            // auth_manager_ = std::make_unique<AuthManager>();  // TODO: Implement AuthManager
            email_queue_ = std::make_unique<EmailQueue>();
            
            // Set up the queue callback
            email_queue_->setSendCallback([this](const Email* email) -> SMTPResult {
                return sendEmailDirect(*email);
            });
            
            is_configured_ = true;
            logger.info("Mailer initialized successfully");
        } catch (const std::exception& e) {
            last_error_ = "Failed to create components: " + std::string(e.what());
            logger.error(last_error_);
        }
}

bool Mailer::Impl::initializeConfiguration(const std::string& config_file) {
    try {
        config_manager_ = std::make_unique<ConfigManager>();
        
        if (config_file.empty()) {
            // Try to load default configuration
            if (!config_manager_->load()) {
                last_error_ = "Failed to load default configuration: " + config_manager_->getLastError();
                return false;
            }
        } else {
            // Load from specified configuration file
            if (!config_manager_->loadFromFile(config_file)) {
                last_error_ = "Failed to load configuration from " + config_file + ": " + 
                              config_manager_->getLastError();
                return false;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        last_error_ = "Configuration initialization failed: " + std::string(e.what());
        return false;
    }
}

SMTPResult Mailer::Impl::send(const Email& email) {
    Logger& logger = Logger::getInstance();
    
    if (!is_configured_) {
        last_error_ = "Mailer not properly configured";
        logger.error(last_error_);
        return SMTPResult::createError(last_error_);
    }
    
    if (!email.isValid()) {
        last_error_ = "Invalid email configuration";
        logger.error(last_error_);
        return SMTPResult::createError(last_error_);
    }
    
    // Validate email permissions
    if (!validateEmailPermissions(email)) {
        last_error_ = "Email permission validation failed";
        logger.error(last_error_);
        return SMTPResult::createError(last_error_);
    }
    
    try {
        // Send email using SMTP client
        SMTPResult result = smtp_client_->send(email);
        
        if (result.success) {
            logger.info("Email sent successfully with message ID: " + result.message_id);
        } else {
            logger.error("Failed to send email: " + result.error_message);
        }
        
        return result;
    } catch (const std::exception& e) {
        last_error_ = "Exception during email sending: " + std::string(e.what());
        logger.error(last_error_);
        return SMTPResult::createError(last_error_);
    }
}

SMTPResult Mailer::Impl::send(const std::string& from, const std::string& to, 
                              const std::string& subject, const std::string& body) {
    Logger& logger = Logger::getInstance();
    logger.debug("Sending simple email from " + from + " to " + to);
    
    // Create Email object
    Email email(from, to, subject, body);
    
    // Send using the main send method
    return send(email);
}

SMTPResult Mailer::Impl::sendHtml(const std::string& from, const std::string& to, 
                                  const std::string& subject, const std::string& body, 
                                  const std::string& html_body) {
    Logger& logger = Logger::getInstance();
    logger.debug("Sending HTML email from " + from + " to " + to);
    
    // Create Email object with HTML content
    Email email(from, to, subject, body);
    email.html_body = html_body;
    
    // Send using the main send method
    return send(email);
}

bool Mailer::Impl::isConfigured() const {
    return is_configured_ && config_manager_ && smtp_client_;
}

std::string Mailer::Impl::getLastError() const {
    return last_error_;
}

bool Mailer::Impl::testConnection() {
    if (!is_configured_) {
        last_error_ = "Mailer not properly configured";
        return false;
    }
    
    try {
        return smtp_client_->testConnection();
    } catch (const std::exception& e) {
        last_error_ = "Connection test failed: " + std::string(e.what());
        return false;
    }
}

bool Mailer::Impl::validateEmailPermissions(const Email& email) {
    if (!config_manager_) {
        last_error_ = "Configuration manager not available";
        return false;
    }
    
    // Validate email addresses and permissions
    std::vector<std::string> to_addresses = email.getAllRecipients();
    if (to_addresses.empty()) {
        last_error_ = "No recipient addresses specified";
        return false;
    }
    
    return config_manager_->validateEmail(email.from, to_addresses);
}

// Queue management implementations
void Mailer::Impl::enqueue(const Email& email, EmailPriority priority) {
    if (!email_queue_) {
        last_error_ = "Email queue not available";
        return;
    }
    
    email_queue_->enqueue(&email, priority);
}

void Mailer::Impl::startQueue() {
    if (!email_queue_) {
        last_error_ = "Email queue not available";
        return;
    }
    
    email_queue_->start();
}

void Mailer::Impl::stopQueue() {
    if (!email_queue_) {
        last_error_ = "Email queue not available";
        return;
    }
    
    email_queue_->stop();
}

bool Mailer::Impl::isQueueRunning() const {
    return email_queue_ ? email_queue_->isRunning() : false;
}

size_t Mailer::Impl::getQueueSize() const {
    return email_queue_ ? email_queue_->size() : 0;
}

std::vector<QueueItem> Mailer::Impl::getPendingEmails() const {
    return email_queue_ ? email_queue_->getPendingEmails() : std::vector<QueueItem>{};
}

std::vector<QueueItem> Mailer::Impl::getFailedEmails() const {
    return email_queue_ ? email_queue_->getFailedEmails() : std::vector<QueueItem>{};
}

SMTPResult Mailer::Impl::sendEmailDirect(const Email& email) {
    // This method is called by the queue to send emails directly
    if (!smtp_client_) {
        return SMTPResult::createError("SMTP client not available");
    }
    
    try {
        return smtp_client_->send(email);
    } catch (const std::exception& e) {
        return SMTPResult::createError("Exception during email sending: " + std::string(e.what()));
    }
}

} // namespace ssmtp_mailer
