#pragma once

#include <string>
#include <vector>
#include <map>

namespace ssmtp_mailer {

/**
 * @brief Command line argument types
 */
enum class ArgumentType {
    FLAG,       // Boolean flag (--help, --version)
    SINGLE,     // Single value (--from, --to)
    MULTIPLE,   // Multiple values (--cc, --bcc)
    OPTIONAL    // Optional value (--config)
};

/**
 * @brief Command line argument definition
 */
struct ArgumentDefinition {
    std::string short_name;
    std::string long_name;
    std::string description;
    ArgumentType type;
    bool required;
    std::string default_value;
    
    ArgumentDefinition(const std::string& short_n, 
                      const std::string& long_n,
                      const std::string& desc,
                      ArgumentType arg_type = ArgumentType::SINGLE,
                      bool req = false,
                      const std::string& def_val = "")
        : short_name(short_n), long_name(long_n), description(desc),
          type(arg_type), required(req), default_value(def_val) {}
};

/**
 * @brief Command line parser class
 */
class CommandLineParser {
public:
    /**
     * @brief Constructor
     */
    CommandLineParser();
    
    /**
     * @brief Destructor
     */
    ~CommandLineParser();
    
    /**
     * @brief Parse command line arguments
     * @param argc Argument count
     * @param argv Argument vector
     * @return true if successful, false otherwise
     */
    bool parse(int argc, char* argv[]);
    
    /**
     * @brief Check if help was requested
     * @return true if help flag present, false otherwise
     */
    bool hasHelp() const;
    
    /**
     * @brief Check if version was requested
     * @return true if version flag present, false otherwise
     */
    bool hasVersion() const;
    
    /**
     * @brief Get from address
     * @return From address string
     */
    std::string getFromAddress() const;
    
    /**
     * @brief Get to addresses
     * @return Vector of to addresses
     */
    std::vector<std::string> getToAddresses() const;
    
    /**
     * @brief Get CC addresses
     * @return Vector of CC addresses
     */
    std::vector<std::string> getCCAddresses() const;
    
    /**
     * @brief Get BCC addresses
     * @return Vector of BCC addresses
     */
    std::vector<std::string> getBCCAddresses() const;
    
    /**
     * @brief Get subject
     * @return Subject string
     */
    std::string getSubject() const;
    
    /**
     * @brief Get body
     * @return Body string
     */
    std::string getBody() const;
    
    /**
     * @brief Get HTML body
     * @return HTML body string
     */
    std::string getHtmlBody() const;
    
    /**
     * @brief Get attachments
     * @return Vector of attachment file paths
     */
    std::vector<std::string> getAttachments() const;
    
    /**
     * @brief Get configuration file path
     * @return Configuration file path
     */
    std::string getConfigFile() const;
    
    /**
     * @brief Get log level
     * @return Log level string
     */
    std::string getLogLevel() const;
    
    /**
     * @brief Get log file path
     * @return Log file path
     */
    std::string getLogFile() const;
    
    /**
     * @brief Get verbosity level
     * @return Verbosity level (0-3)
     */
    int getVerbosity() const;
    
    /**
     * @brief Check if dry run mode is enabled
     * @return true if dry run, false otherwise
     */
    bool isDryRun() const;
    
    /**
     * @brief Check if debug mode is enabled
     * @return true if debug mode, false otherwise
     */
    bool isDebugMode() const;
    
    /**
     * @brief Get custom argument value
     * @param name Argument name (long or short)
     * @return Argument value or empty string if not found
     */
    std::string getArgument(const std::string& name) const;
    
    /**
     * @brief Get custom argument values
     * @param name Argument name (long or short)
     * @return Vector of argument values
     */
    std::vector<std::string> getArguments(const std::string& name) const;
    
    /**
     * @brief Check if argument is present
     * @param name Argument name (long or short)
     * @return true if present, false otherwise
     */
    bool hasArgument(const std::string& name) const;
    
    /**
     * @brief Print usage information
     */
    void printUsage() const;
    
    /**
     * @brief Print version information
     */
    void printVersion() const;
    
    /**
     * @brief Get last error message
     * @return Last error message
     */
    std::string getLastError() const;
    
    /**
     * @brief Check if parsing was successful
     * @return true if successful, false otherwise
     */
    bool isValid() const;

private:
    /**
     * @brief Initialize argument definitions
     */
    void initializeArguments();
    
    /**
     * @brief Parse argument value
     * @param arg_name Argument name
     * @param arg_value Argument value
     * @return true if successful, false otherwise
     */
    bool parseArgument(const std::string& arg_name, const std::string& arg_value);
    
    /**
     * @brief Parse flag argument
     * @param arg_name Argument name
     * @return true if successful, false otherwise
     */
    bool parseFlag(const std::string& arg_name);
    
    /**
     * @brief Parse single value argument
     * @param arg_name Argument name
     * @param arg_value Argument value
     * @return true if successful, false otherwise
     */
    bool parseSingleValue(const std::string& arg_name, const std::string& arg_value);
    
    /**
     * @brief Parse multiple value argument
     * @param arg_name Argument name
     * @param arg_value Argument value
     * @return true if successful, false otherwise
     */
    bool parseMultipleValue(const std::string& arg_name, const std::string& arg_value);
    
    /**
     * @brief Validate required arguments
     * @return true if all required arguments present, false otherwise
     */
    bool validateRequiredArguments() const;
    
    /**
     * @brief Validate email addresses
     * @return true if all email addresses valid, false otherwise
     */
    bool validateEmailAddresses() const;
    
    /**
     * @brief Split comma-separated values
     * @param value Comma-separated string
     * @return Vector of individual values
     */
    std::vector<std::string> splitValues(const std::string& value) const;
    
    /**
     * @brief Trim whitespace from string
     * @param str String to trim
     * @return Trimmed string
     */
    std::string trim(const std::string& str) const;
    
    /**
     * @brief Check if string is valid email address
     * @param email Email address to validate
     * @return true if valid, false otherwise
     */
    bool isValidEmailAddress(const std::string& email) const;
    
    /**
     * @brief Find argument definition by name
     * @param name Argument name (long or short)
     * @return Pointer to argument definition or nullptr if not found
     */
    const ArgumentDefinition* findArgumentDefinition(const std::string& name) const;

private:
    std::vector<ArgumentDefinition> argument_definitions_;
    std::map<std::string, std::string> single_values_;
    std::map<std::string, std::vector<std::string>> multiple_values_;
    std::map<std::string, bool> flags_;
    
    // Parsed values
    std::string from_address_;
    std::vector<std::string> to_addresses_;
    std::vector<std::string> cc_addresses_;
    std::vector<std::string> bcc_addresses_;
    std::string subject_;
    std::string body_;
    std::string html_body_;
    std::vector<std::string> attachments_;
    std::string config_file_;
    std::string log_level_;
    std::string log_file_;
    int verbosity_;
    bool dry_run_;
    bool debug_mode_;
    
    std::string last_error_;
    bool is_valid_;
    bool help_requested_;
    bool version_requested_;
};

} // namespace ssmtp_mailer
