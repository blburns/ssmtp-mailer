#include "ssmtp-mailer/cli_manager.hpp"
#include "core/config/config_manager.hpp"
#include "ssmtp-mailer/cli_commands.hpp"
#include "core/logging/logger.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>

namespace ssmtp_mailer {

CLIManager::CLIManager() : initialized_(false) {
}

CLIManager::~CLIManager() {
}

bool CLIManager::initialize() {
    if (initialized_) {
        return true;
    }
    
    try {
        registerCommands();
        initialized_ = true;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize CLI manager: " << e.what() << std::endl;
        return false;
    }
}

CLIResult CLIManager::executeCommand(const std::string& command, const std::vector<std::string>& args) {
    if (!initialized_) {
        return CLIResult::error_result("CLI manager not initialized");
    }
    
    auto it = commands_.find(command);
    if (it == commands_.end()) {
        // Check aliases
        for (const auto& cmd : commands_) {
            if (std::find(cmd.second.aliases.begin(), cmd.second.aliases.end(), command) != cmd.second.aliases.end()) {
                return cmd.second.handler(args);
            }
        }
        return CLIResult::error_result("Unknown command: " + command);
    }
    
    try {
        return it->second.handler(args);
    } catch (const std::exception& e) {
        return CLIResult::error_result("Command execution failed: " + std::string(e.what()));
    }
}

std::map<std::string, std::string> CLIManager::getAvailableCommands() const {
    std::map<std::string, std::string> result;
    for (const auto& cmd : commands_) {
        result[cmd.first] = cmd.second.description;
    }
    return result;
}

std::string CLIManager::getCommandHelp(const std::string& command) const {
    auto it = commands_.find(command);
    if (it == commands_.end()) {
        return "Command not found: " + command;
    }
    
    std::string help = it->second.description + "\n\n";
    help += "Usage: " + it->second.usage + "\n";
    
    if (!it->second.aliases.empty()) {
        help += "Aliases: ";
        for (size_t i = 0; i < it->second.aliases.size(); ++i) {
            if (i > 0) help += ", ";
            help += it->second.aliases[i];
        }
        help += "\n";
    }
    
    return help;
}

void CLIManager::printHelp() const {
    std::cout << "\nssmtp-mailer CLI - Configuration Management Tool\n";
    std::cout << "================================================\n\n";
    
    std::cout << "Available Commands:\n\n";
    
    // Group commands by category
    std::map<std::string, std::vector<std::pair<std::string, std::string>>> categories;
    
    for (const auto& cmd : commands_) {
        std::string category = "General";
        if (cmd.first.find("config") == 0) {
            category = "Configuration";
        } else if (cmd.first.find("auth") == 0) {
            category = "Authentication";
        } else if (cmd.first.find("template") == 0) {
            category = "Templates";
        } else if (cmd.first.find("api") == 0) {
            category = "API Management";
        } else if (cmd.first.find("validate") == 0 || cmd.first.find("test") == 0) {
            category = "Validation & Testing";
        } else if (cmd.first.find("setup") == 0) {
            category = "Setup & Wizards";
        }
        
        categories[category].push_back({cmd.first, cmd.second.description});
    }
    
    for (const auto& category : categories) {
        std::cout << category.first << ":\n";
        for (const auto& cmd : category.second) {
            std::cout << "  " << std::setw(20) << std::left << cmd.first 
                      << " " << cmd.second << "\n";
        }
        std::cout << "\n";
    }
    
    std::cout << "Use 'ssmtp-mailer cli <command> --help' for detailed help on a specific command.\n";
}

bool CLIManager::commandExists(const std::string& command) const {
    if (commands_.find(command) != commands_.end()) {
        return true;
    }
    
    // Check aliases
    for (const auto& cmd : commands_) {
        if (std::find(cmd.second.aliases.begin(), cmd.second.aliases.end(), command) != cmd.second.aliases.end()) {
            return true;
        }
    }
    
    return false;
}

void CLIManager::registerCommands() {
    registerConfigCommands();
    registerAuthCommands();
    registerTemplateCommands();
    registerAPICommands();
    registerValidationCommands();
    registerSetupCommands();
}

void CLIManager::registerConfigCommands() {
    // Domain management commands
    commands_["config-domain-add"] = CLICommand(
        "config-domain-add",
        "Add a new domain configuration",
        "config domain add <domain> --smtp-server <server> --port <port> [options]",
        [](const std::vector<std::string>& args) -> CLIResult { return ConfigCommands::addDomain(args); }
    );
    
    commands_["config-domain-list"] = CLICommand(
        "config-domain-list",
        "List all configured domains",
        "config domain list",
        [](const std::vector<std::string>& args) -> CLIResult { return ConfigCommands::listDomains(args); }
    );
    
    commands_["config-domain-show"] = CLICommand(
        "config-domain-show",
        "Show domain configuration details",
        "config domain show <domain>",
        [](const std::vector<std::string>& args) -> CLIResult { return ConfigCommands::showDomain(args); }
    );
    
    // User management commands
    commands_["config-user-add"] = CLICommand(
        "config-user-add",
        "Add a new user configuration",
        "config user add <email> --domain <domain> [options]",
        [](const std::vector<std::string>& args) -> CLIResult { return ConfigCommands::addUser(args); }
    );
    
    commands_["config-user-list"] = CLICommand(
        "config-user-list",
        "List all configured users",
        "config user list [--domain <domain>]",
        [](const std::vector<std::string>& args) -> CLIResult { return ConfigCommands::listUsers(args); }
    );
    
    // Global configuration commands
    commands_["config-global-show"] = CLICommand(
        "config-global-show",
        "Show global configuration",
        "config global show",
        [](const std::vector<std::string>& args) -> CLIResult { return ConfigCommands::showGlobalConfig(args); }
    );
}

void CLIManager::registerAuthCommands() {
    commands_["auth-oauth2-setup"] = CLICommand(
        "auth-oauth2-setup",
        "Setup OAuth2 authentication for a domain",
        "auth oauth2 setup --provider <google|microsoft> --domain <domain>",
        [](const std::vector<std::string>& args) -> CLIResult { return AuthCommands::setupOAuth2(args); }
    );
    
    commands_["auth-oauth2-test"] = CLICommand(
        "auth-oauth2-test",
        "Test OAuth2 authentication for a domain",
        "auth oauth2 test --domain <domain>",
        [](const std::vector<std::string>& args) -> CLIResult { return AuthCommands::testOAuth2(args); }
    );
    
    commands_["auth-service-account-add"] = CLICommand(
        "auth-service-account-add",
        "Add a service account configuration",
        "auth service-account add --domain <domain> --file <json-file>",
        [](const std::vector<std::string>& args) -> CLIResult { return AuthCommands::addServiceAccount(args); }
    );
    
    commands_["auth-service-account-list"] = CLICommand(
        "auth-service-account-list",
        "List all service account configurations",
        "auth service-account list",
        [](const std::vector<std::string>& args) -> CLIResult { return AuthCommands::listServiceAccounts(args); }
    );
    
    commands_["auth-service-account-test"] = CLICommand(
        "auth-service-account-test",
        "Test service account configuration",
        "auth service-account test --domain <domain>",
        [](const std::vector<std::string>& args) -> CLIResult { return AuthCommands::testServiceAccount(args); }
    );
}

void CLIManager::registerTemplateCommands() {
    commands_["template-create"] = CLICommand(
        "template-create",
        "Create a new email template",
        "template create <name> --subject <subject> --body <body> [--html <html>]",
        [](const std::vector<std::string>& args) -> CLIResult { return TemplateCommands::createTemplate(args); }
    );
    
    commands_["template-list"] = CLICommand(
        "template-list",
        "List all email templates",
        "template list",
        [](const std::vector<std::string>& args) -> CLIResult { return TemplateCommands::listTemplates(args); }
    );
    
    commands_["template-show"] = CLICommand(
        "template-show",
        "Show template details",
        "template show <name>",
        [](const std::vector<std::string>& args) -> CLIResult { return TemplateCommands::showTemplate(args); }
    );
    
    commands_["template-test"] = CLICommand(
        "template-test",
        "Test a template with sample data",
        "template test <name> --to <email> [--from <email>]",
        [](const std::vector<std::string>& args) -> CLIResult { return TemplateCommands::testTemplate(args); }
    );
    
    commands_["template-address-create"] = CLICommand(
        "template-address-create",
        "Create an address template",
        "template address create <pattern> --domain <domain> --types <types>",
        [](const std::vector<std::string>& args) -> CLIResult { return TemplateCommands::createAddressTemplate(args); }
    );
    
    commands_["template-address-list"] = CLICommand(
        "template-address-list",
        "List all address templates",
        "template address list",
        [](const std::vector<std::string>& args) -> CLIResult { return TemplateCommands::listAddressTemplates(args); }
    );
}

void CLIManager::registerAPICommands() {
    commands_["api-provider-add"] = CLICommand(
        "api-provider-add",
        "Add a new API provider configuration",
        "api provider add <provider> --api-key <key> --sender <email>",
        [](const std::vector<std::string>& args) -> CLIResult {
            return CLIResult::success_result("API provider management not yet implemented");
        }
    );
    
    commands_["api-provider-list"] = CLICommand(
        "api-provider-list",
        "List all configured API providers",
        "api provider list",
        [](const std::vector<std::string>& args) -> CLIResult {
            std::cout << "API provider management not yet implemented\n";
            return CLIResult::success_result();
        }
    );
}

void CLIManager::registerValidationCommands() {
    commands_["validate-config"] = CLICommand(
        "validate-config",
        "Validate all configuration files",
        "validate config [--fix] [--verbose]",
        [](const std::vector<std::string>& args) -> CLIResult { return ValidationCommands::validateConfig(args); }
    );
    
    commands_["test-connections"] = CLICommand(
        "test-connections",
        "Test all SMTP and API connections",
        "test connections [--smtp-only] [--api-only] [--domain <domain>]",
        [](const std::vector<std::string>& args) -> CLIResult { return ValidationCommands::testConnections(args); }
    );
    
    commands_["config-backup"] = CLICommand(
        "config-backup",
        "Backup all configuration files",
        "config backup [--file <backup-file>]",
        [](const std::vector<std::string>& args) -> CLIResult { return ValidationCommands::backupConfig(args); }
    );
    
    commands_["config-restore"] = CLICommand(
        "config-restore",
        "Restore configuration from backup",
        "config restore --file <backup-file>",
        [](const std::vector<std::string>& args) -> CLIResult { return ValidationCommands::restoreConfig(args); }
    );
}

void CLIManager::registerSetupCommands() {
    commands_["setup-wizard"] = CLICommand(
        "setup-wizard",
        "Run interactive setup wizard",
        "setup wizard",
        [](const std::vector<std::string>& args) -> CLIResult { return SetupWizard::runWizard(args); }
    );
    
    commands_["setup-domain"] = CLICommand(
        "setup-domain",
        "Setup a domain interactively",
        "setup domain <domain>",
        [](const std::vector<std::string>& args) -> CLIResult { return SetupWizard::setupDomain(args); }
    );
}

} // namespace ssmtp_mailer
