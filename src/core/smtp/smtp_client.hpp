#pragma once

#include <string>
#include <vector>
#include <memory>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "core/config/config_manager.hpp"
#include "utils/email.hpp"

namespace ssmtp_mailer {

/**
 * @brief SMTP connection state
 */
enum class SMTPState {
    DISCONNECTED,
    CONNECTED,
    AUTHENTICATED,
    MAIL_FROM_SENT,
    RCPT_TO_SENT,
    DATA_SENT,
    QUIT_SENT
};

/**
 * @brief SMTP authentication methods
 */
enum class SMTPAuthMethod {
    NONE,
    LOGIN,
    PLAIN,
    CRAM_MD5,
    OAUTH2,
    XOAUTH2
};

/**
 * @brief SMTP client class for handling SMTP connections and sending emails
 */
class SMTPClient {
public:
    /**
     * @brief Constructor
     * @param config Configuration manager instance
     */
    explicit SMTPClient(const ConfigManager& config);
    
    /**
     * @brief Destructor
     */
    ~SMTPClient();
    
    /**
     * @brief Send an email
     * @param email Email object to send
     * @return SMTPResult with operation status
     */
    SMTPResult send(const Email& email);
    
    /**
     * @brief Connect to SMTP server
     * @param server SMTP server hostname
     * @param port SMTP server port
     * @param use_ssl Whether to use SSL
     * @return true if successful, false otherwise
     */
    bool connect(const std::string& server, int port, bool use_ssl = false);
    
    /**
     * @brief Disconnect from SMTP server
     */
    void disconnect();
    
    /**
     * @brief Authenticate with SMTP server
     * @param username Username for authentication
     * @param password Password for authentication
     * @param auth_method Authentication method to use
     * @return true if successful, false otherwise
     */
    bool authenticate(const std::string& username, 
                     const std::string& password, 
                     SMTPAuthMethod auth_method = SMTPAuthMethod::LOGIN);
    
    /**
     * @brief Authenticate using OAuth2
     * @param username Username (usually email)
     * @param oauth2_token OAuth2 access token
     * @return true if successful, false otherwise
     */
    bool authenticateOAuth2(const std::string& username, const std::string& oauth2_token);
    
    /**
     * @brief Check if connected to SMTP server
     * @return true if connected, false otherwise
     */
    bool isConnected() const;
    
    /**
     * @brief Check if authenticated
     * @return true if authenticated, false otherwise
     */
    bool isAuthenticated() const;
    
    /**
     * @brief Get current SMTP state
     * @return Current SMTP state
     */
    SMTPState getState() const;
    
    /**
     * @brief Get last error message
     * @return Last error message
     */
    std::string getLastError() const;
    
    /**
     * @brief Test SMTP connection
     * @return true if connection successful, false otherwise
     */
    bool testConnection();

private:
    /**
     * @brief Initialize SSL context
     * @return true if successful, false otherwise
     */
    bool initializeSSL();
    
    /**
     * @brief Clean up SSL context
     */
    void cleanupSSL();
    
    /**
     * @brief Send SMTP command
     * @param command SMTP command to send
     * @return true if successful, false otherwise
     */
    bool sendCommand(const std::string& command);
    
    /**
     * @brief Read SMTP response
     * @param response Response string to store result
     * @return SMTP response code
     */
    int readResponse(std::string& response);
    
    /**
     * @brief Check SMTP response code
     * @param code Response code to check
     * @return true if successful (2xx or 3xx), false otherwise
     */
    bool isResponseSuccess(int code) const;
    
    /**
     * @brief Send EHLO command
     * @param hostname Hostname to send
     * @return true if successful, false otherwise
     */
    bool sendEHLO(const std::string& hostname);
    
    /**
     * @brief Send STARTTLS command
     * @return true if successful, false otherwise
     */
    bool sendSTARTTLS();
    
    /**
     * @brief Send MAIL FROM command
     * @param from_address From address
     * @return true if successful, false otherwise
     */
    bool sendMAILFROM(const std::string& from_address);
    
    /**
     * @brief Send RCPT TO command
     * @param to_address To address
     * @return true if successful, false otherwise
     */
    bool sendRCPTTO(const std::string& to_address);
    
    /**
     * @brief Send DATA command and email content
     * @param email Email object to send
     * @return true if successful, false otherwise
     */
    bool sendDATA(const Email& email);
    
    /**
     * @brief Send QUIT command
     * @return true if successful, false otherwise
     */
    bool sendQUIT();
    
    /**
     * @brief Format email headers
     * @param email Email object
     * @return Formatted email headers
     */
    std::string formatEmailHeaders(const Email& email) const;
    
    /**
     * @brief Format email body
     * @param email Email object
     * @return Formatted email body
     */
    std::string formatEmailBody(const Email& email) const;
    
    /**
     * @brief Generate Message-ID
     * @return Generated Message-ID
     */
    std::string generateMessageID() const;
    
    /**
     * @brief Get current timestamp
     * @return Current timestamp string
     */
    std::string getCurrentTimestamp() const;
    
    /**
     * @brief Write data to socket
     * @param data Data to write
     * @param length Length of data
     * @return Number of bytes written
     */
    int writeData(const char* data, size_t length);
    
    /**
     * @brief Read data from socket
     * @param buffer Buffer to store data
     * @param max_length Maximum length to read
     * @return Number of bytes read
     */
    int readData(char* buffer, size_t max_length);

private:
    const ConfigManager& config_;
    int socket_fd_;
    SSL* ssl_;
    SSL_CTX* ssl_ctx_;
    SMTPState state_;
    std::string server_;
    int port_;
    bool use_ssl_;
    std::string last_error_;
    std::string capabilities_;
    
    // SSL configuration
    std::string ssl_cert_file_;
    std::string ssl_key_file_;
    std::string ssl_ca_file_;
    
    // Connection settings
    int connection_timeout_;
    int read_timeout_;
    int write_timeout_;
};

} // namespace ssmtp_mailer
