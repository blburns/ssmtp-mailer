#include "simple-smtp-mailer/cli_commands.hpp"
#include "simple-smtp-mailer/config_utils.hpp"
#include "../logging/logger.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <ctime>
#include <iomanip>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"

namespace ssmtp_mailer {

// Interactive setup wizard

CLIResult SetupWizard::runWizard(const std::vector<std::string>& args) {
        std::cout << "\n";
        std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                simple-smtp-mailer Setup Wizard                    ║\n";
        std::cout << "║                                                              ║\n";
        std::cout << "║  This wizard will guide you through the initial setup of    ║\n";
        std::cout << "║  your simple-smtp-mailer configuration.                           ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════════╝\n\n";
        
        try {
            // Step 1: Global Configuration
            if (!setupGlobalConfig()) {
                return CLIResult::error_result("Setup cancelled by user");
            }
            
            // Step 2: Domain Configuration
            if (!setupDomains()) {
                return CLIResult::error_result("Setup cancelled by user");
            }
            
            // Step 3: User Configuration
            if (!setupUsers()) {
                return CLIResult::error_result("Setup cancelled by user");
            }
            
            // Step 4: Authentication Setup
            if (!setupAuthentication()) {
                return CLIResult::error_result("Setup cancelled by user");
            }
            
            // Step 5: API Configuration
            if (!setupAPIProviders()) {
                return CLIResult::error_result("Setup cancelled by user");
            }
            
            // Step 6: Validation and Testing
            if (!validateAndTest()) {
                return CLIResult::error_result("Setup validation failed");
            }
            
            std::cout << "\n";
            std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
            std::cout << "║                    Setup Complete!                         ║\n";
            std::cout << "║                                                              ║\n";
            std::cout << "║  Your simple-smtp-mailer is now configured and ready to use.     ║\n";
            std::cout << "║  You can start sending emails with:                         ║\n";
            std::cout << "║                                                              ║\n";
            std::cout << "║    simple-smtp-mailer send --from user@domain.com \\              ║\n";
            std::cout << "║                --to recipient@example.com \\                ║\n";
            std::cout << "║                --subject 'Hello' \\                         ║\n";
            std::cout << "║                --body 'Hello World!'                        ║\n";
            std::cout << "╚══════════════════════════════════════════════════════════════╝\n\n";
            
            return CLIResult::success_result();
            
        } catch (const std::exception& e) {
            return CLIResult::error_result("Setup wizard failed: " + std::string(e.what()));
        }
    }
    
CLIResult SetupWizard::setupDomain(const std::vector<std::string>& args) {
        if (args.empty()) {
            return CLIResult::error_result("Usage: setup domain <domain>");
        }
        
        std::string domain = args[0];
        
        std::cout << "\nSetting up domain: " << domain << "\n";
        std::cout << "==================\n\n";
        
        try {
            // Get SMTP server information
            std::string smtp_server = promptInput("SMTP Server", "smtp.gmail.com");
            int port = std::stoi(promptInput("SMTP Port", "587"));
            
            // Get authentication method
            std::cout << "\nAuthentication Method:\n";
            std::cout << "1. Username/Password (LOGIN)\n";
            std::cout << "2. OAuth2 (Google/Microsoft)\n";
            std::cout << "3. Service Account (Google)\n";
            
            int auth_choice = std::stoi(promptInput("Choose authentication method (1-3)", "1"));
            
            std::string auth_method;
            std::string username;
            std::string password;
            
            switch (auth_choice) {
                case 1:
                    auth_method = "LOGIN";
                    username = promptInput("Username", "mailer@" + domain);
                    password = promptPassword("Password");
                    break;
                case 2:
                    auth_method = "OAUTH2";
                    username = promptInput("Username", "mailer@" + domain);
                    std::cout << "OAuth2 setup will be configured separately.\n";
                    break;
                case 3:
                    auth_method = "SERVICE_ACCOUNT";
                    username = promptInput("Service Account Email", "mailer@" + domain);
                    std::cout << "Service account file will be configured separately.\n";
                    break;
                default:
                    return CLIResult::error_result("Invalid authentication method choice");
            }
            
            // SSL/TLS settings
            std::cout << "\nSSL/TLS Settings:\n";
            std::cout << "1. Use STARTTLS (recommended for port 587)\n";
            std::cout << "2. Use SSL (for port 465)\n";
            std::cout << "3. No encryption (not recommended)\n";
            
            int ssl_choice = std::stoi(promptInput("Choose SSL/TLS method (1-3)", "1"));
            
            bool use_ssl = false;
            bool use_starttls = false;
            
            switch (ssl_choice) {
                case 1:
                    use_starttls = true;
                    break;
                case 2:
                    use_ssl = true;
                    break;
                case 3:
                    // No encryption
                    break;
                default:
                    return CLIResult::error_result("Invalid SSL/TLS choice");
            }
            
            // Create domain configuration
            std::vector<std::string> domain_args = {
                domain,
                "--smtp-server", smtp_server,
                "--port", std::to_string(port),
                "--auth-method", auth_method,
                "--username", username
            };
            
            if (!password.empty()) {
                domain_args.push_back("--password");
                domain_args.push_back(password);
            }
            
            if (use_ssl) {
                domain_args.push_back("--use-ssl");
            } else if (use_starttls) {
                domain_args.push_back("--use-starttls");
            }
            
            return ConfigCommands::addDomain(domain_args);
            
        } catch (const std::exception& e) {
            return CLIResult::error_result("Domain setup failed: " + std::string(e.what()));
        }
    }

bool SetupWizard::setupGlobalConfig() {
        std::cout << "Step 1: Global Configuration\n";
        std::cout << "============================\n\n";
        
        std::string default_hostname = promptInput("Default hostname", "localhost");
        std::string default_from = promptInput("Default from address", "noreply@" + default_hostname);
        std::string log_level = promptInput("Log level (DEBUG/INFO/WARN/ERROR)", "INFO");
        
        // Create global configuration
        std::string config_dir = ConfigUtils::getConfigDirectory();
        if (!ConfigUtils::ensureConfigDirectory(config_dir)) {
            std::cerr << "Failed to create configuration directory: " << config_dir << std::endl;
            return false;
        }
        
        std::string config_file = config_dir + "/simple-smtp-mailer.conf";
        std::ofstream file(config_file);
        
        if (!file.is_open()) {
            std::cerr << "Failed to create global configuration file\n";
            return false;
        }
        
        file << "# simple-smtp-mailer Global Configuration\n";
        file << "# Generated by setup wizard\n\n";
        file << "[global]\n";
        file << "default_hostname = " << default_hostname << "\n";
        file << "default_from = " << default_from << "\n";
        file << "config_dir = " << config_dir << "\n";
        file << "domains_dir = " << config_dir << "/domains\n";
        file << "users_dir = " << config_dir << "/users\n";
        file << "mappings_dir = " << config_dir << "/mappings\n";
        file << "ssl_dir = " << config_dir << "/ssl\n";
        file << "log_file = /var/log/simple-smtp-mailer.log\n";
        file << "log_level = " << log_level << "\n";
        file << "max_connections = 10\n";
        file << "connection_timeout = 30\n";
        file << "read_timeout = 60\n";
        file << "write_timeout = 60\n";
        file << "enable_rate_limiting = true\n";
        file << "rate_limit_per_minute = 100\n";
        
        file.close();
        
        std::cout << "✓ Global configuration created: " << config_file << "\n\n";
        return true;
    }
    
bool SetupWizard::setupDomains() {
        std::cout << "Step 2: Domain Configuration\n";
        std::cout << "============================\n\n";
        
        std::cout << "Do you want to configure domains now? (y/n): ";
        std::string response;
        std::getline(std::cin, response);
        
        if (response != "y" && response != "Y" && response != "yes") {
            std::cout << "Skipping domain configuration. You can add domains later.\n\n";
            return true;
        }
        
        while (true) {
            std::string domain = promptInput("Domain name (or 'done' to finish)", "");
            
            if (domain == "done" || domain.empty()) {
                break;
            }
            
            std::vector<std::string> domain_args = {domain};
            auto result = setupDomain(domain_args);
            
            if (!result.success) {
                std::cerr << "Failed to setup domain: " << result.message << "\n";
                std::cout << "Do you want to continue? (y/n): ";
                std::getline(std::cin, response);
                if (response != "y" && response != "Y" && response != "yes") {
                    return false;
                }
            } else {
                std::cout << "✓ Domain '" << domain << "' configured successfully\n";
            }
        }
        
        std::cout << "\n";
        return true;
    }
    
bool SetupWizard::setupUsers() {
        std::cout << "Step 3: User Configuration\n";
        std::cout << "==========================\n\n";
        
        std::cout << "Do you want to configure users now? (y/n): ";
        std::string response;
        std::getline(std::cin, response);
        
        if (response != "y" && response != "Y" && response != "yes") {
            std::cout << "Skipping user configuration. You can add users later.\n\n";
            return true;
        }
        
        while (true) {
            std::string email = promptInput("User email (or 'done' to finish)", "");
            
            if (email == "done" || email.empty()) {
                break;
            }
            
            std::string domain = promptInput("Domain for this user", "");
            
            if (domain.empty()) {
                std::cerr << "Domain is required for user configuration\n";
                continue;
            }
            
            std::vector<std::string> user_args = {
                email,
                "--domain", domain
            };
            
            auto result = ConfigCommands::addUser(user_args);
            
            if (!result.success) {
                std::cerr << "Failed to setup user: " << result.message << "\n";
                std::cout << "Do you want to continue? (y/n): ";
                std::getline(std::cin, response);
                if (response != "y" && response != "Y" && response != "yes") {
                    return false;
                }
            } else {
                std::cout << "✓ User '" << email << "' configured successfully\n";
            }
        }
        
        std::cout << "\n";
        return true;
    }
    
bool SetupWizard::setupAuthentication() {
        std::cout << "Step 4: Authentication Setup\n";
        std::cout << "============================\n\n";
        
        std::cout << "Do you want to configure OAuth2 or Service Account authentication? (y/n): ";
        std::string response;
        std::getline(std::cin, response);
        
        if (response != "y" && response != "Y" && response != "yes") {
            std::cout << "Skipping authentication setup. You can configure it later.\n\n";
            return true;
        }
        
        std::cout << "Authentication Options:\n";
        std::cout << "1. OAuth2 (Google/Microsoft)\n";
        std::cout << "2. Service Account (Google)\n";
        
        int auth_choice = std::stoi(promptInput("Choose authentication method (1-2)", "1"));
        
        switch (auth_choice) {
            case 1:
                return setupOAuth2();
            case 2:
                return setupServiceAccount();
            default:
                std::cerr << "Invalid authentication choice\n";
                return false;
        }
    }
    
bool SetupWizard::setupOAuth2() {
        std::string domain = promptInput("Domain for OAuth2", "");
        if (domain.empty()) {
            std::cerr << "Domain is required for OAuth2 setup\n";
            return false;
        }
        
        std::cout << "OAuth2 Provider:\n";
        std::cout << "1. Google\n";
        std::cout << "2. Microsoft\n";
        
        int provider_choice = std::stoi(promptInput("Choose provider (1-2)", "1"));
        
        std::string provider = (provider_choice == 1) ? "google" : "microsoft";
        
        std::vector<std::string> oauth2_args = {
            "--provider", provider,
            "--domain", domain
        };
        
        auto result = AuthCommands::setupOAuth2(oauth2_args);
        
        if (!result.success) {
            std::cerr << "OAuth2 setup failed: " << result.message << "\n";
            return false;
        }
        
        std::cout << "✓ OAuth2 setup completed for " << domain << "\n\n";
        return true;
    }
    
bool SetupWizard::setupServiceAccount() {
        std::string domain = promptInput("Domain for Service Account", "");
        if (domain.empty()) {
            std::cerr << "Domain is required for Service Account setup\n";
            return false;
        }
        
        std::string service_account_file = promptInput("Path to service account JSON file", "");
        if (service_account_file.empty()) {
            std::cerr << "Service account file path is required\n";
            return false;
        }
        
        std::vector<std::string> sa_args = {
            "--domain", domain,
            "--file", service_account_file
        };
        
        auto result = AuthCommands::addServiceAccount(sa_args);
        
        if (!result.success) {
            std::cerr << "Service Account setup failed: " << result.message << "\n";
            return false;
        }
        
        std::cout << "✓ Service Account setup completed for " << domain << "\n\n";
        return true;
    }
    
bool SetupWizard::setupAPIProviders() {
        std::cout << "Step 5: API Provider Configuration\n";
        std::cout << "==================================\n\n";
        
        std::cout << "Do you want to configure API providers (SendGrid, Mailgun, etc.)? (y/n): ";
        std::string response;
        std::getline(std::cin, response);
        
        if (response != "y" && response != "Y" && response != "yes") {
            std::cout << "Skipping API provider configuration. You can add them later.\n\n";
            return true;
        }
        
        // TODO: Implement API provider setup
        std::cout << "API provider setup will be implemented in a future version.\n\n";
        return true;
    }
    
bool SetupWizard::validateAndTest() {
        std::cout << "Step 6: Validation and Testing\n";
        std::cout << "==============================\n\n";
        
        std::cout << "Validating configuration...\n";
        
        std::vector<std::string> validate_args = {"--verbose"};
        auto result = ValidationCommands::validateConfig(validate_args);
        
        if (!result.success) {
            std::cerr << "Configuration validation failed: " << result.message << "\n";
            std::cout << "Do you want to continue anyway? (y/n): ";
            std::string response;
            std::getline(std::cin, response);
            if (response != "y" && response != "Y" && response != "yes") {
                return false;
            }
        } else {
            std::cout << "✓ Configuration validation passed\n";
        }
        
        std::cout << "\nTesting connections...\n";
        
        std::vector<std::string> test_args;
        auto test_result = ValidationCommands::testConnections(test_args);
        
        if (!test_result.success) {
            std::cerr << "Connection testing failed: " << test_result.message << "\n";
            std::cout << "You can test connections later with: simple-smtp-mailer test connections\n";
        } else {
            std::cout << "✓ Connection testing completed\n";
        }
        
        std::cout << "\n";
        return true;
    }
    
std::string SetupWizard::promptInput(const std::string& prompt, const std::string& default_value) {
        std::cout << prompt;
        if (!default_value.empty()) {
            std::cout << " [" << default_value << "]";
        }
        std::cout << ": ";
        
        std::string input;
        std::getline(std::cin, input);
        
        if (input.empty() && !default_value.empty()) {
            return default_value;
        }
        
        return input;
    }
    
std::string SetupWizard::promptPassword(const std::string& prompt) {
        std::cout << prompt << ": ";
        
        // TODO: Implement hidden password input
        // For now, just use regular input
        std::string password;
        std::getline(std::cin, password);
        
        return password;
    }

} // namespace ssmtp_mailer

#pragma GCC diagnostic pop
