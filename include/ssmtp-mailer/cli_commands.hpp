#pragma once

#include "ssmtp-mailer/cli_manager.hpp"

namespace ssmtp_mailer {

/**
 * @brief Configuration command implementations
 */
class ConfigCommands {
public:
    static CLIResult addDomain(const std::vector<std::string>& args);
    static CLIResult listDomains(const std::vector<std::string>& args);
    static CLIResult showDomain(const std::vector<std::string>& args);
    static CLIResult addUser(const std::vector<std::string>& args);
    static CLIResult listUsers(const std::vector<std::string>& args);
    static CLIResult showGlobalConfig(const std::vector<std::string>& args);
};

/**
 * @brief Authentication command implementations
 */
class AuthCommands {
public:
    static CLIResult setupOAuth2(const std::vector<std::string>& args);
    static CLIResult testOAuth2(const std::vector<std::string>& args);
    static CLIResult addServiceAccount(const std::vector<std::string>& args);
    static CLIResult listServiceAccounts(const std::vector<std::string>& args);
    static CLIResult testServiceAccount(const std::vector<std::string>& args);
};

/**
 * @brief Template command implementations
 */
class TemplateCommands {
public:
    static CLIResult createTemplate(const std::vector<std::string>& args);
    static CLIResult listTemplates(const std::vector<std::string>& args);
    static CLIResult showTemplate(const std::vector<std::string>& args);
    static CLIResult testTemplate(const std::vector<std::string>& args);
    static CLIResult createAddressTemplate(const std::vector<std::string>& args);
    static CLIResult listAddressTemplates(const std::vector<std::string>& args);
};

/**
 * @brief Validation command implementations
 */
class ValidationCommands {
public:
    static CLIResult validateConfig(const std::vector<std::string>& args);
    static CLIResult testConnections(const std::vector<std::string>& args);
    static CLIResult backupConfig(const std::vector<std::string>& args);
    static CLIResult restoreConfig(const std::vector<std::string>& args);
};

/**
 * @brief Setup wizard
 */
class SetupWizard {
public:
    static CLIResult runWizard(const std::vector<std::string>& args);
    static CLIResult setupDomain(const std::vector<std::string>& args);

private:
    static bool setupGlobalConfig();
    static bool setupDomains();
    static bool setupUsers();
    static bool setupAuthentication();
    static bool setupOAuth2();
    static bool setupServiceAccount();
    static bool setupAPIProviders();
    static bool validateAndTest();
    static std::string promptInput(const std::string& prompt, const std::string& default_value);
    static std::string promptPassword(const std::string& prompt);
};

} // namespace ssmtp_mailer
