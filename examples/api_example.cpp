#include <iostream>
#include <string>
#include "ssmtp-mailer/unified_mailer.hpp"

int main() {
    std::cout << "ssmtp-mailer API Example" << std::endl;
    std::cout << "=========================" << std::endl;
    
    try {
        // Create unified mailer configuration
        ssmtp_mailer::UnifiedMailerConfig config;
        config.default_method = ssmtp_mailer::SendMethod::API;
        config.enable_fallback = true;  // Fall back to SMTP if API fails
        
        // Configure SendGrid API provider
        ssmtp_mailer::APIClientConfig sendgrid_config;
        sendgrid_config.provider = ssmtp_mailer::APIProvider::SENDGRID;
        sendgrid_config.auth.api_key = "your_sendgrid_api_key_here";
        sendgrid_config.sender_email = "your-verified-sender@yourdomain.com";
        sendgrid_config.sender_name = "Your Company";
        sendgrid_config.request.base_url = "https://api.sendgrid.com";
        sendgrid_config.request.endpoint = "/v3/mail/send";
        sendgrid_config.request.timeout_seconds = 30;
        sendgrid_config.enable_tracking = true;
        
        // Add SendGrid configuration
        config.api_configs["sendgrid"] = sendgrid_config;
        
        // Create unified mailer
        ssmtp_mailer::UnifiedMailer mailer(config);
        
        // Create test email
        ssmtp_mailer::Email email;
        email.from = "your-verified-sender@yourdomain.com";
        email.to.push_back("recipient@example.com");
        email.subject = "Test Email from API";
        email.body = "This is a test email sent via the SendGrid API.";
        email.html_body = "<h1>Test Email</h1><p>This is a test email sent via the <strong>SendGrid API</strong>.</p>";
        
        std::cout << "Sending email..." << std::endl;
        std::cout << "From: " << email.from << std::endl;
        std::cout << "To: " << email.to[0] << std::endl;
        std::cout << "Subject: " << email.subject << std::endl;
        
        // Send email
        auto result = mailer.sendEmail(email);
        
        if (result.success) {
            std::cout << "\n✓ Email sent successfully!" << std::endl;
            std::cout << "Method used: " << (result.method_used == ssmtp_mailer::SendMethod::API ? "API" : "SMTP") << std::endl;
            std::cout << "Provider: " << result.provider_name << std::endl;
            std::cout << "Message ID: " << result.message_id << std::endl;
        } else {
            std::cout << "\n✗ Failed to send email" << std::endl;
            std::cout << "Error: " << result.error_message << std::endl;
            std::cout << "Method used: " << (result.method_used == ssmtp_mailer::SendMethod::API ? "API" : "SMTP") << std::endl;
        }
        
        // Show statistics
        std::cout << "\nSending Statistics:" << std::endl;
        auto stats = mailer.getStatistics();
        for (const auto& stat : stats) {
            std::cout << "  " << stat.first << ": " << stat.second << std::endl;
        }
        
        // Test API connection
        std::cout << "\nTesting API connection..." << std::endl;
        if (mailer.testConnection(ssmtp_mailer::SendMethod::API, "sendgrid")) {
            std::cout << "✓ API connection successful" << std::endl;
        } else {
            std::cout << "✗ API connection failed" << std::endl;
        }
        
        // Show available providers
        std::cout << "\nAvailable API Providers:" << std::endl;
        auto providers = mailer.getAvailableAPIProviders();
        for (const auto& provider : providers) {
            std::cout << "  - " << provider << std::endl;
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
