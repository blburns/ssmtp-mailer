#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cstdlib>
#include <cstring>

#include "core/config/config_manager.hpp"
#include "core/smtp/smtp_client.hpp"
#include "core/logging/logger.hpp"
#include "utils/email.hpp"
#include "utils/command_line.hpp"

using namespace ssmtp_mailer;

int main(int argc, char* argv[]) {
    try {
        // Initialize logging
        Logger::initialize();
        Logger& logger = Logger::getInstance();
        
        logger.info("Starting ssmtp-mailer v1.0.0");
        
        // Parse command line arguments
        CommandLineParser parser;
        if (!parser.parse(argc, argv)) {
            std::cerr << "Error parsing command line arguments" << std::endl;
            parser.printUsage();
            return EXIT_FAILURE;
        }
        
        // Show help if requested
        if (parser.hasHelp()) {
            parser.printUsage();
            return EXIT_SUCCESS;
        }
        
        // Show version if requested
        if (parser.hasVersion()) {
            std::cout << "ssmtp-mailer v1.0.0" << std::endl;
            return EXIT_SUCCESS;
        }
        
        // Load configuration
        ConfigManager config;
        if (!config.load()) {
            logger.error("Failed to load configuration");
            return EXIT_FAILURE;
        }
        
        // Create SMTP client
        std::unique_ptr<SMTPClient> smtp_client = std::make_unique<SMTPClient>(config);
        
        // Create email object
        Email email;
        email.from = parser.getFromAddress();
        email.to = parser.getToAddresses();
        email.subject = parser.getSubject();
        email.body = parser.getBody();
        email.html_body = parser.getHtmlBody();
        
        // Validate email
        if (!email.isValid()) {
            logger.error("Invalid email configuration");
            std::cerr << "Error: Invalid email configuration" << std::endl;
            return EXIT_FAILURE;
        }
        
        // Send email
        logger.info("Sending email from " + email.from + " to " + email.to[0]);
        
        SMTPResult result = smtp_client->send(email);
        
        if (result.success) {
            logger.info("Email sent successfully");
            std::cout << "Email sent successfully" << std::endl;
            return EXIT_SUCCESS;
        } else {
            logger.error("Failed to send email: " + result.error_message);
            std::cerr << "Error: " << result.error_message << std::endl;
            return EXIT_FAILURE;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        return EXIT_FAILURE;
    }
}
