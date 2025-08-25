#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>
#include <openssl/rsa.h>

namespace ssmtp_mailer {

/**
 * Simple Service Account Authentication for Gmail SMTP
 * 
 * This class handles Google Service Account authentication using JWT tokens.
 * It automatically generates and refreshes access tokens for Gmail SMTP access.
 * 
 * Note: This is a simplified version that doesn't require external JSON libraries.
 */
class ServiceAccountAuthSimple {
public:
    /**
     * Constructor
     * @param service_account_file Path to the service account JSON file
     * @param user_email Email address to impersonate (for domain-wide delegation)
     */
    ServiceAccountAuthSimple(const std::string& service_account_file, 
                           const std::string& user_email);
    
    /**
     * Destructor
     */
    ~ServiceAccountAuthSimple();
    
    /**
     * Generate a new access token for Gmail SMTP
     * @return Access token string
     * @throws std::runtime_error if token generation fails
     */
    std::string generateAccessToken();
    
    /**
     * Check if the current token is expired
     * @return true if token is expired or will expire within 5 minutes
     */
    bool isTokenExpired() const;
    
    /**
     * Get current access token (refresh if needed)
     * @return Valid access token string
     */
    std::string getAccessToken();
    
    /**
     * Get the user email being impersonated
     * @return User email address
     */
    std::string getUserEmail() const { return user_email_; }
    
    /**
     * Get the service account client email
     * @return Service account email address
     */
    std::string getClientEmail() const { return client_email_; }
    
    /**
     * Check if service account is properly loaded
     * @return true if service account is valid
     */
    bool isValid() const { return !private_key_.empty() && !client_email_.empty(); }
    
    /**
     * Get token expiry time
     * @return Unix timestamp when token expires
     */
    time_t getTokenExpiry() const { return token_expiry_; }
    
    /**
     * Get remaining token lifetime in seconds
     * @return Seconds until token expires
     */
    int getTokenLifetime() const;

private:
    std::string service_account_file_;
    std::string user_email_;
    std::string private_key_;
    std::string client_email_;
    std::string client_id_;
    std::string token_uri_;
    
    std::string current_token_;
    time_t token_expiry_;
    
    // JWT token generation
    std::string createJWT();
    std::string base64UrlEncode(const std::string& input);
    std::string signJWT(const std::string& header, const std::string& payload);
    
    // Load service account JSON (simple parsing)
    bool loadServiceAccount();
    
    // Exchange JWT for access token
    std::string exchangeJWTForToken(const std::string& jwt);
    
    // Utility functions
    std::string createJWTHeader();
    std::string createJWTPayload();
    std::string rsaSign(const std::string& data);
    
    // Simple JSON parsing helpers
    std::string extractJsonValue(const std::string& json, const std::string& key);
    std::string extractQuotedValue(const std::string& json, const std::string& key);
    
    // Cleanup OpenSSL resources
    void cleanupOpenSSL();
};

} // namespace ssmtp_mailer
