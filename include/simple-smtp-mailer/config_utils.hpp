#pragma once

#include <string>
#include <filesystem>

namespace ssmtp_mailer {

/**
 * @brief Configuration directory utilities
 */
class ConfigUtils {
public:
    /**
     * @brief Get the appropriate configuration directory
     * @param prefer_user If true, prefer user-level config over system config
     * @return Path to configuration directory
     */
    static std::string getConfigDirectory(bool prefer_user = true);
    
    /**
     * @brief Get the domains configuration directory
     * @param prefer_user If true, prefer user-level config over system config
     * @return Path to domains configuration directory
     */
    static std::string getDomainsDirectory(bool prefer_user = true);
    
    /**
     * @brief Get the users configuration directory
     * @param prefer_user If true, prefer user-level config over system config
     * @return Path to users configuration directory
     */
    static std::string getUsersDirectory(bool prefer_user = true);
    
    /**
     * @brief Get the mappings configuration directory
     * @param prefer_user If true, prefer user-level config over system config
     * @return Path to mappings configuration directory
     */
    static std::string getMappingsDirectory(bool prefer_user = true);
    
    /**
     * @brief Get the SSL certificates directory
     * @param prefer_user If true, prefer user-level config over system config
     * @return Path to SSL certificates directory
     */
    static std::string getSSLDirectory(bool prefer_user = true);
    
    /**
     * @brief Check if running as root
     * @return true if running as root, false otherwise
     */
    static bool isRunningAsRoot();
    
    /**
     * @brief Ensure configuration directory exists
     * @param config_dir Path to configuration directory
     * @return true if successful, false otherwise
     */
    static bool ensureConfigDirectory(const std::string& config_dir);
    
    /**
     * @brief Get user home directory
     * @return Path to user home directory
     */
    static std::string getUserHomeDirectory();
    
    /**
     * @brief Get the default system configuration directory
     * @return Path to system configuration directory
     */
    static std::string getSystemConfigDirectory();
    
    /**
     * @brief Get the default user configuration directory
     * @return Path to user configuration directory
     */
    static std::string getUserConfigDirectory();

private:
    static std::string getEnvironmentVariable(const std::string& name);
};

} // namespace ssmtp_mailer
