#include "simple-smtp-mailer/config_utils.hpp"
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>

namespace ssmtp_mailer {

std::string ConfigUtils::getConfigDirectory(bool prefer_user) {
    // Check for environment variable first
    std::string env_config = getEnvironmentVariable("SSMTP_MAILER_CONFIG_DIR");
    if (!env_config.empty()) {
        return env_config;
    }
    
    if (prefer_user) {
        // Try user config first
        std::string user_config = getUserConfigDirectory();
        if (std::filesystem::exists(user_config) || !isRunningAsRoot()) {
            return user_config;
        }
    }
    
    // Fall back to system config
    return getSystemConfigDirectory();
}

std::string ConfigUtils::getDomainsDirectory(bool prefer_user) {
    std::string config_dir = getConfigDirectory(prefer_user);
    return config_dir + "/domains";
}

std::string ConfigUtils::getUsersDirectory(bool prefer_user) {
    std::string config_dir = getConfigDirectory(prefer_user);
    return config_dir + "/users";
}

std::string ConfigUtils::getMappingsDirectory(bool prefer_user) {
    std::string config_dir = getConfigDirectory(prefer_user);
    return config_dir + "/mappings";
}

std::string ConfigUtils::getSSLDirectory(bool prefer_user) {
    std::string config_dir = getConfigDirectory(prefer_user);
    return config_dir + "/ssl";
}

bool ConfigUtils::isRunningAsRoot() {
    return getuid() == 0;
}

bool ConfigUtils::ensureConfigDirectory(const std::string& config_dir) {
    try {
        std::filesystem::create_directories(config_dir);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to create configuration directory: " << e.what() << std::endl;
        return false;
    }
}

std::string ConfigUtils::getUserHomeDirectory() {
    const char* home = getenv("HOME");
    if (home != nullptr) {
        return std::string(home);
    }
    
    // Fallback to getpwuid
    struct passwd* pw = getpwuid(getuid());
    if (pw != nullptr && pw->pw_dir != nullptr) {
        return std::string(pw->pw_dir);
    }
    
    return "";
}

std::string ConfigUtils::getSystemConfigDirectory() {
    return "/etc/simple-smtp-mailer";
}

std::string ConfigUtils::getUserConfigDirectory() {
    std::string home = getUserHomeDirectory();
    if (home.empty()) {
        return "";
    }
    return home + "/.config/simple-smtp-mailer";
}

std::string ConfigUtils::getEnvironmentVariable(const std::string& name) {
    const char* value = getenv(name.c_str());
    if (value != nullptr) {
        return std::string(value);
    }
    return "";
}

} // namespace ssmtp_mailer
