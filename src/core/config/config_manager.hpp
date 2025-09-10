#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <unordered_map>

namespace ssmtp_mailer {

/**
 * @brief Configuration section representing a domain
 */
struct DomainConfig {
    std::string name;
    bool enabled;
    std::string smtp_server;
    int smtp_port;
    std::string auth_method;
    std::string service_account;
    std::string relay_account;
    std::string username;
    std::string password;
    std::string oauth2_token;
    bool use_ssl;
    bool use_starttls;
    std::string ssl_cert_file;
    std::string ssl_key_file;
    std::string ssl_ca_file;
    
    DomainConfig() : enabled(true), smtp_port(587), use_ssl(false), use_starttls(true) {}
};

/**
 * @brief Configuration section representing a user
 */
struct UserConfig {
    std::string email;
    std::string domain;
    bool enabled;
    bool can_send_from;
    bool can_send_to;
    bool template_address;
    std::vector<std::string> allowed_types;
    std::vector<std::string> allowed_recipients;
    std::vector<std::string> allowed_domains;
    
    UserConfig() : enabled(true), can_send_from(true), can_send_to(true), template_address(false) {}
};

/**
 * @brief Configuration section representing address mapping
 */
struct AddressMapping {
    std::string from_pattern;
    std::string to_pattern;
    std::string smtp_account;
    std::string domain;
    std::vector<std::string> allowed_recipients;
    
    AddressMapping() = default;
};

/**
 * @brief Global configuration settings
 */
struct GlobalConfig {
    std::string default_hostname;
    std::string default_from;
    std::string config_dir;
    std::string domains_dir;
    std::string users_dir;
    std::string mappings_dir;
    std::string ssl_dir;
    std::string log_file;
    std::string log_level;
    int max_connections;
    int connection_timeout;
    int read_timeout;
    int write_timeout;
    bool enable_rate_limiting;
    int rate_limit_per_minute;
    
    GlobalConfig() : max_connections(10), connection_timeout(30), 
                     read_timeout(60), write_timeout(60), 
                     enable_rate_limiting(true), rate_limit_per_minute(100) {}
};

/**
 * @brief Main configuration manager class
 */
class ConfigManager {
public:
    /**
     * @brief Constructor
     */
    ConfigManager();
    
    /**
     * @brief Destructor
     */
    ~ConfigManager();
    
    /**
     * @brief Load configuration from files
     * @return true if successful, false otherwise
     */
    bool load();
    
    /**
     * @brief Load configuration from specific file
     * @param config_file Path to configuration file
     * @return true if successful, false otherwise
     */
    bool loadFromFile(const std::string& config_file);
    
    /**
     * @brief Get global configuration
     * @return Reference to global configuration
     */
    const GlobalConfig& getGlobalConfig() const;
    
    /**
     * @brief Get domain configuration by name
     * @param domain_name Domain name
     * @return Pointer to domain configuration or nullptr if not found
     */
    const DomainConfig* getDomainConfig(const std::string& domain_name) const;
    
    /**
     * @brief Get user configuration by email
     * @param email User email address
     * @return Pointer to user configuration or nullptr if not found
     */
    const UserConfig* getUserConfig(const std::string& email) const;
    
    /**
     * @brief Get address mapping for a from address
     * @param from_address From address to map
     * @return Pointer to address mapping or nullptr if not found
     */
    const AddressMapping* getAddressMapping(const std::string& from_address) const;
    
    /**
     * @brief Get all domain configurations
     * @return Vector of domain configurations
     */
    std::vector<DomainConfig> getAllDomainConfigs() const;
    
    /**
     * @brief Get all user configurations
     * @return Vector of user configurations
     */
    std::vector<UserConfig> getAllUserConfigs() const;
    
    /**
     * @brief Check if configuration is valid
     * @return true if valid, false otherwise
     */
    bool isValid() const;
    
    /**
     * @brief Get last error message
     * @return Last error message
     */
    std::string getLastError() const;
    
    /**
     * @brief Reload configuration
     * @return true if successful, false otherwise
     */
    bool reload();
    
    /**
     * @brief Validate email address against configuration
     * @param from_address From address to validate
     * @param to_addresses To addresses to validate
     * @return true if valid, false otherwise
     */
    bool validateEmail(const std::string& from_address, 
                      const std::vector<std::string>& to_addresses) const;

private:
    /**
     * @brief Load main configuration file
     * @param config_file Path to main config file
     * @return true if successful, false otherwise
     */
    bool loadMainConfig(const std::string& config_file);
    
    /**
     * @brief Load configuration directory
     * @param config_dir Path to configuration directory
     * @return true if successful, false otherwise
     */
    bool loadConfigDirectory(const std::string& config_dir);
    
    /**
     * @brief Load domain configurations
     * @param domains_dir Path to domains directory
     * @return true if successful, false otherwise
     */
    bool loadDomainConfigs(const std::string& domains_dir);
    
    /**
     * @brief Load user configurations
     * @param users_dir Path to users directory
     * @return true if successful, false otherwise
     */
    bool loadUserConfigs(const std::string& users_dir);
    
    /**
     * @brief Load address mappings
     * @param mappings_dir Path to mappings directory
     * @return true if successful, false otherwise
     */
    bool loadAddressMappings(const std::string& mappings_dir);
    
    /**
     * @brief Parse configuration file
     * @param file_path Path to configuration file
     * @return true if successful, false otherwise
     */
    bool parseConfigFile(const std::string& file_path);
    
    /**
     * @brief Parse domain configuration section
     * @param section_name Section name
     * @param key_value_pairs Key-value pairs from section
     * @return true if successful, false otherwise
     */
    bool parseDomainConfig(const std::string& section_name, 
                          const std::map<std::string, std::string>& key_value_pairs);
    
    /**
     * @brief Parse user configuration section
     * @param section_name Section name
     * @param key_value_pairs Key-value pairs from section
     * @return true if successful, false otherwise
     */
    bool parseUserConfig(const std::string& section_name, 
                        const std::map<std::string, std::string>& key_value_pairs);
    
    /**
     * @brief Parse address mapping section
     * @param section_name Section name
     * @param key_value_pairs Key-value pairs from section
     * @return true if successful, false otherwise
     */
    bool parseAddressMapping(const std::string& section_name, 
                           const std::map<std::string, std::string>& key_value_pairs);
    
    /**
     * @brief Parse global configuration section
     * @param key_value_pairs Key-value pairs from section
     * @return true if successful, false otherwise
     */
    bool parseGlobalConfig(const std::map<std::string, std::string>& key_value_pairs);
    
    /**
     * @brief Extract domain from email address
     * @param email Email address
     * @return Domain name
     */
    std::string extractDomain(const std::string& email) const;
    
    /**
     * @brief Check if string matches pattern
     * @param str String to check
     * @param pattern Pattern to match against
     * @return true if matches, false otherwise
     */
    bool matchesPattern(const std::string& str, const std::string& pattern) const;
    
    /**
     * @brief Parse configuration section
     * @param section_name Section name
     * @param key_value_pairs Key-value pairs from section
     * @return true if successful, false otherwise
     */
    bool parseSection(const std::string& section_name, 
                     const std::map<std::string, std::string>& key_value_pairs);
    
    /**
     * @brief Set up default configurations
     */
    void setupDefaultConfigs();

private:
    GlobalConfig global_config_;
    std::unordered_map<std::string, DomainConfig> domain_configs_;
    std::unordered_map<std::string, UserConfig> user_configs_;
    std::unordered_map<std::string, AddressMapping> address_mappings_;
    mutable std::string last_error_;
    bool is_valid_;
};

} // namespace ssmtp_mailer
