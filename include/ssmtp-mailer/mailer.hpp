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
     * @brief Default constructor
     */
    Email() = default;
    
    /**
     * @brief Constructor with basic fields
     * @param from_addr From address
     * @param to_addr To address
     * @param subject_line Subject line
     * @param body_text Body text
     */
    Email(const std::string& from_addr, 
          const std::string& to_addr, 
          const std::string& subject_line, 
          const std::string& body_text);
    
    /**
     * @brief Constructor with multiple recipients
     * @param from_addr From address
     * @param to_addrs Vector of to addresses
     * @param subject_line Subject line
     * @param body_text Body text
     */
    Email(const std::string& from_addr, 
          const std::vector<std::string>& to_addrs, 
          const std::string& subject_line, 
          const std::string& body_text);
    
    /**
     * @brief Check if the email configuration is valid
     * @return true if valid, false otherwise
     */
    bool isValid() const;
    
    /**
     * @brief Clear all email fields
     */
    void clear();
    
    /**
     * @brief Add recipient address
     * @param address Recipient address to add
     */
    void addRecipient(const std::string& address);
    
    /**
     * @brief Add CC address
     * @param address CC address to add
     */
    void addCC(const std::string& address);
    
    /**
     * @brief Add BCC address
     * @param address BCC address to add
     */
    void addBCC(const std::string& address);
    
    /**
     * @brief Add attachment
     * @param file_path Path to attachment file
     */
    void addAttachment(const std::string& file_path);
    
    /**
     * @brief Remove recipient address
     * @param address Recipient address to remove
     * @return true if removed, false if not found
     */
    bool removeRecipient(const std::string& address);
    
    /**
     * @brief Remove CC address
     * @param address CC address to remove
     * @return true if removed, false if not found
     */
    bool removeCC(const std::string& address);
    
    /**
     * @brief Remove BCC address
     * @param address BCC address to remove
     * @return true if removed, false if not found
     */
    bool removeBCC(const std::string& address);
    
    /**
     * @brief Remove attachment
     * @param file_path Path to attachment file
     * @return true if removed, false if not found
     */
    bool removeAttachment(const std::string& file_path);
    
    /**
     * @brief Get all recipient addresses (to + cc + bcc)
     * @return Vector of all recipient addresses
     */
    std::vector<std::string> getAllRecipients() const;
    
    /**
     * @brief Check if address is valid email format
     * @param address Email address to validate
     * @return true if valid format, false otherwise
     */
    static bool isValidEmailAddress(const std::string& address);
    
    /**
     * @brief Extract domain from email address
     * @param address Email address
     * @return Domain name
     */
    static std::string extractDomain(const std::string& address);
    
    /**
     * @brief Extract username from email address
     * @param address Email address
     * @return Username part
     */
    static std::string extractUsername(const std::string& address);
    
    /**
     * @brief Normalize email address (lowercase, trim whitespace)
     * @param address Email address to normalize
     * @return Normalized email address
     */
    static std::string normalizeEmailAddress(const std::string& address);
    
    /**
     * @brief Check if email has HTML content
     * @return true if HTML body is present, false otherwise
     */
    bool hasHtmlContent() const;
    
    /**
     * @brief Check if email has attachments
     * @return true if attachments are present, false otherwise
     */
    bool hasAttachments() const;
    
    /**
     * @brief Get email size estimate in bytes
     * @return Estimated size in bytes
     */
    size_t getEstimatedSize() const;
    
    /**
     * @brief Generate Message-ID header value
     * @return Generated Message-ID
     */
    std::string generateMessageId() const;
    
    /**
     * @brief Get current timestamp for Date header
     * @return RFC 2822 formatted timestamp
     */
    static std::string getCurrentTimestamp();
    
    /**
     * @brief Convert email to RFC 2822 format
     * @return RFC 2822 formatted email
     */
    std::string toRFC2822() const;
    
    /**
     * @brief Convert email to MIME format
     * @return MIME formatted email
     */
    std::string toMIME() const;
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
    
    /**
     * @brief Create successful result
     * @param msg_id Message ID
     * @return Successful SMTP result
     */
    static SMTPResult success(const std::string& msg_id = "");
    
    /**
     * @brief Create error result
     * @param error_msg Error message
     * @param error_code Error code
     * @return Error SMTP result
     */
    static SMTPResult error(const std::string& error_msg, int error_code = 0);
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
