#include "core/config/config_manager.hpp"
#include <stdexcept>

namespace ssmtp_mailer {

ConfigManager::ConfigManager() {
    // Stub implementation
}

ConfigManager::~ConfigManager() {
    // Stub implementation
}

bool ConfigManager::loadFromFile(const std::string& config_file) {
    (void)config_file; // Suppress unused parameter warning
    // Stub implementation - always return true for now
    return true;
}

bool ConfigManager::load() {
    // Stub implementation - always return true for now
    return true;
}

std::string ConfigManager::getLastError() const {
    return "No errors";
}

bool ConfigManager::validateEmail(const std::string& from, const std::vector<std::string>& to) const {
    (void)from; // Suppress unused parameter warning
    (void)to;   // Suppress unused parameter warning
    // Stub implementation - always return true for now
    return true;
}

} // namespace ssmtp_mailer
