#pragma once

#include <string>
#include <vector>
#include <regex>

namespace ssmtp_mailer {

// Note: SMTPResult and Email structs are defined in include/ssmtp-mailer/mailer.hpp
// This file provides utility functions for email operations

/**
 * @brief Check if address is valid email format
 * @param address Email address to validate
 * @return true if valid format, false otherwise
 */
bool isValidEmailAddress(const std::string& address);

/**
 * @brief Extract domain from email address
 * @param address Email address
 * @return Domain name
 */
std::string extractDomain(const std::string& address);

/**
 * @brief Extract username from email address
 * @param address Email address
 * @return Username part
 */
std::string extractUsername(const std::string& address);

/**
 * @brief Normalize email address (lowercase, trim whitespace)
 * @param address Email address to normalize
 * @return Normalized email address
 */
std::string normalizeEmailAddress(const std::string& address);

/**
 * @brief Generate unique identifier
 * @return Unique identifier string
 */
std::string generateUniqueId();

/**
 * @brief Get current timestamp for Date header
 * @return RFC 2822 formatted timestamp
 */
std::string getCurrentTimestamp();

} // namespace ssmtp_mailer
