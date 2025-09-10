#include "core/config/config_manager.hpp"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>

namespace ssmtp_mailer {

ConfigManager::ConfigManager() : is_valid_(false) {
    // Initialize with default values
}

ConfigManager::~ConfigManager() {
    // Cleanup if needed
}

bool ConfigManager::loadFromFile(const std::string& config_file) {
    (void)config_file; // Suppress unused parameter warning
    // For now, just set up some default domain configs
    setupDefaultConfigs();
    is_valid_ = true;
    return true;
}

bool ConfigManager::load() {
    // For now, just set up some default domain configs
    setupDefaultConfigs();
    is_valid_ = true;
    return true;
}

std::string ConfigManager::getLastError() const {
    return last_error_;
}

bool ConfigManager::validateEmail(const std::string& from, const std::vector<std::string>& to) const {
    (void)from; // Suppress unused parameter warning
    (void)to;   // Suppress unused parameter warning
    // Stub implementation - always return true for now
    return true;
}

const GlobalConfig& ConfigManager::getGlobalConfig() const {
    return global_config_;
}

const DomainConfig* ConfigManager::getDomainConfig(const std::string& domain_name) const {
    auto it = domain_configs_.find(domain_name);
    if (it != domain_configs_.end()) {
        return &it->second;
    }
    return nullptr;
}

const UserConfig* ConfigManager::getUserConfig(const std::string& email) const {
    auto it = user_configs_.find(email);
    if (it != user_configs_.end()) {
        return &it->second;
    }
    return nullptr;
}

const AddressMapping* ConfigManager::getAddressMapping(const std::string& from_address) const {
    auto it = address_mappings_.find(from_address);
    if (it != address_mappings_.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<DomainConfig> ConfigManager::getAllDomainConfigs() const {
    std::vector<DomainConfig> result;
    for (const auto& pair : domain_configs_) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<UserConfig> ConfigManager::getAllUserConfigs() const {
    std::vector<UserConfig> result;
    for (const auto& pair : user_configs_) {
        result.push_back(pair.second);
    }
    return result;
}

bool ConfigManager::isValid() const {
    return is_valid_;
}

bool ConfigManager::reload() {
    return load();
}

void ConfigManager::setupDefaultConfigs() {
    // Set up some default domain configurations for common email providers
    
    // Gmail configuration
    DomainConfig gmail_config;
    gmail_config.name = "gmail.com";
    gmail_config.enabled = true;
    gmail_config.smtp_server = "smtp.gmail.com";
    gmail_config.smtp_port = 587;
    gmail_config.auth_method = "LOGIN";
    gmail_config.use_ssl = false;
    gmail_config.use_starttls = true;
    domain_configs_["gmail.com"] = gmail_config;
    
    // Outlook/Hotmail configuration
    DomainConfig outlook_config;
    outlook_config.name = "outlook.com";
    outlook_config.enabled = true;
    outlook_config.smtp_server = "smtp-mail.outlook.com";
    outlook_config.smtp_port = 587;
    outlook_config.auth_method = "LOGIN";
    outlook_config.use_ssl = false;
    outlook_config.use_starttls = true;
    domain_configs_["outlook.com"] = outlook_config;
    
    // Yahoo configuration
    DomainConfig yahoo_config;
    yahoo_config.name = "yahoo.com";
    yahoo_config.enabled = true;
    yahoo_config.smtp_server = "smtp.mail.yahoo.com";
    yahoo_config.smtp_port = 587;
    yahoo_config.auth_method = "LOGIN";
    yahoo_config.use_ssl = false;
    yahoo_config.use_starttls = true;
    domain_configs_["yahoo.com"] = yahoo_config;
    
    // Generic SMTP configuration for other domains
    DomainConfig generic_config;
    generic_config.name = "generic";
    generic_config.enabled = true;
    generic_config.smtp_server = "localhost";
    generic_config.smtp_port = 25;
    generic_config.auth_method = "NONE";
    generic_config.use_ssl = false;
    generic_config.use_starttls = false;
    domain_configs_["generic"] = generic_config;
}

} // namespace ssmtp_mailer
