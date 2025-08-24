#include "utils/email.hpp"
#include <sstream>
#include <iomanip>
#include <chrono>
#include <random>
#include <algorithm>
#include <cctype>

namespace ssmtp_mailer {

// Static member initialization
static const std::regex email_regex_(
    R"(([a-zA-Z0-9._%+-]+)@([a-zA-Z0-9.-]+\.[a-zA-Z]{2,}))"
);

// Utility functions
bool isValidEmailAddress(const std::string& address) {
    if (address.empty()) return false;
    
    std::smatch match;
    return std::regex_match(address, match, email_regex_);
}

std::string extractDomain(const std::string& address) {
    std::smatch match;
    if (std::regex_match(address, match, email_regex_)) {
        return match[2].str();
    }
    return "";
}

std::string extractUsername(const std::string& address) {
    std::smatch match;
    if (std::regex_match(address, match, email_regex_)) {
        return match[1].str();
    }
    return "";
}

std::string normalizeEmailAddress(const std::string& address) {
    std::string normalized = address;
    
    // Trim whitespace
    normalized.erase(0, normalized.find_first_not_of(" \t\r\n"));
    normalized.erase(normalized.find_last_not_of(" \t\r\n") + 1);
    
    // Convert to lowercase
    std::transform(normalized.begin(), normalized.end(), normalized.begin(), ::tolower);
    
    return normalized;
}

std::string generateUniqueId() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    
    std::ostringstream oss;
    oss << std::hex;
    
    for (int i = 0; i < 32; ++i) {
        oss << dis(gen);
    }
    
    return oss.str();
}

std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::gmtime(&time_t);
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%a, %d %b %Y %H:%M:%S +0000");
    return oss.str();
}

} // namespace ssmtp_mailer
