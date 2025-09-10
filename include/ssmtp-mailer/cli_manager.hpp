#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace ssmtp_mailer {

/**
 * @brief CLI command result
 */
struct CLIResult {
    bool success;
    std::string message;
    int exit_code;
    
    CLIResult() : success(false), exit_code(1) {}
    CLIResult(bool s, const std::string& msg, int code = 0) 
        : success(s), message(msg), exit_code(code) {}
    
    static CLIResult success_result(const std::string& msg = "") {
        return CLIResult(true, msg, 0);
    }
    
    static CLIResult error_result(const std::string& msg, int code = 1) {
        return CLIResult(false, msg, code);
    }
};

/**
 * @brief CLI command definition
 */
struct CLICommand {
    std::string name;
    std::string description;
    std::string usage;
    std::vector<std::string> aliases;
    std::function<CLIResult(const std::vector<std::string>&)> handler;
    
    CLICommand(const std::string& n, const std::string& desc, 
               const std::string& use,
               std::function<CLIResult(const std::vector<std::string>&)> h)
        : name(n), description(desc), usage(use), handler(h) {}
};

/**
 * @brief CLI Manager for handling all configuration commands
 */
class CLIManager {
public:
    /**
     * @brief Constructor
     */
    CLIManager();
    
    /**
     * @brief Destructor
     */
    ~CLIManager();
    
    /**
     * @brief Initialize the CLI manager
     * @return true if successful, false otherwise
     */
    bool initialize();
    
    /**
     * @brief Execute a command
     * @param command Command name
     * @param args Command arguments
     * @return CLI result
     */
    CLIResult executeCommand(const std::string& command, const std::vector<std::string>& args);
    
    /**
     * @brief Get available commands
     * @return Map of command names to descriptions
     */
    std::map<std::string, std::string> getAvailableCommands() const;
    
    /**
     * @brief Get command help
     * @param command Command name
     * @return Help string
     */
    std::string getCommandHelp(const std::string& command) const;
    
    /**
     * @brief Print general help
     */
    void printHelp() const;
    
    /**
     * @brief Check if command exists
     * @param command Command name
     * @return true if exists, false otherwise
     */
    bool commandExists(const std::string& command) const;

private:
    /**
     * @brief Register all CLI commands
     */
    void registerCommands();
    
    /**
     * @brief Register configuration commands
     */
    void registerConfigCommands();
    
    /**
     * @brief Register authentication commands
     */
    void registerAuthCommands();
    
    /**
     * @brief Register template commands
     */
    void registerTemplateCommands();
    
    /**
     * @brief Register API commands
     */
    void registerAPICommands();
    
    /**
     * @brief Register validation commands
     */
    void registerValidationCommands();
    
    /**
     * @brief Register setup commands
     */
    void registerSetupCommands();

private:
    std::map<std::string, CLICommand> commands_;
    bool initialized_;
};

} // namespace ssmtp_mailer
