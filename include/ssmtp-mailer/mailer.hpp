#pragma once

#include <string>
#include <vector>
#include <memory>

namespace ssmtp_mailer {

/**
 * @brief Email structure for composing messages
 */
struct Email {
    std::string from;
    std::vector<std::string> to;
    std::vector<std::string> cc;
    std::vector<std::string> bcc;
    std::string subject;
    std::string body;
    std::string html_body;
    std::vector<std::string> attachments;
    
    /**
     * @brief Check if the email configuration is valid
     * @return true if valid, false otherwise
     */
    bool isValid() const;
    
    /**
     * @brief Clear all email fields
     */
    void clear();
};

/**
 * @brief Result of SMTP operations
 */
struct SMTPResult {
    bool success;
    std::string message_id;
    std::string error_message;
    int error_code;
    
    SMTPResult() : success(false), error_code(0) {}
};

/**
 * @brief Main mailer class for sending emails
 */
class Mailer {
public:
    /**
     * @brief Constructor
     * @param config_file Path to configuration file (optional)
     */
    explicit Mailer(const std::string& config_file = "");
    
    /**
     * @brief Destructor
     */
    ~Mailer();
    
    /**
     * @brief Send an email
     * @param email Email object to send
     * @return SMTPResult with operation status
     */
    SMTPResult send(const Email& email);
    
    /**
     * @brief Send an email with simple parameters
     * @param from Sender address
     * @param to Recipient address(es)
     * @param subject Email subject
     * @param body Email body
     * @return SMTPResult with operation status
     */
    SMTPResult send(const std::string& from, 
                    const std::string& to, 
                    const std::string& subject, 
                    const std::string& body);
    
    /**
     * @brief Send an email with HTML content
     * @param from Sender address
     * @param to Recipient address(es)
     * @param subject Email subject
     * @param body Plain text body
     * @param html_body HTML body
     * @return SMTPResult with operation status
     */
    SMTPResult sendHtml(const std::string& from, 
                       const std::string& to, 
                       const std::string& subject, 
                       const std::string& body, 
                       const std::string& html_body);
    
    /**
     * @brief Check if the mailer is properly configured
     * @return true if configured, false otherwise
     */
    bool isConfigured() const;
    
    /**
     * @brief Get the last error message
     * @return Last error message
     */
    std::string getLastError() const;
    
    /**
     * @brief Test SMTP connection
     * @return true if connection successful, false otherwise
     */
    bool testConnection();

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace ssmtp_mailer
