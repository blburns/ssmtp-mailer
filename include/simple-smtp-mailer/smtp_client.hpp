#pragma once

#include <string>
#include <vector>
#include <memory>
#include "simple-smtp-mailer/mailer.hpp"

namespace ssmtp_mailer {

// Forward declarations
class ConfigManager;

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
     * @brief Test connection to SMTP server
     * @return true if successful, false otherwise
     */
    bool testConnection();
    
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

private:
    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace ssmtp_mailer
