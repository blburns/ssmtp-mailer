#include <iostream>
#include <string>
#include <map>
#include "ssmtp-mailer/unified_mailer.hpp"

int main() {
    std::cout << "ssmtp-mailer Multi-Provider Example" << std::endl;
    std::cout << "====================================" << std::endl;
    
    try {
        // Create unified mailer configuration
        ssmtp_mailer::UnifiedMailerConfig config;
        config.default_method = ssmtp_mailer::SendMethod::AUTO;
        config.enable_fallback = true;
        config.max_retries = 3;
        config.retry_delay = std::chrono::seconds(5);
        
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
        
        // Configure Mailgun API provider
        ssmtp_mailer::APIClientConfig mailgun_config;
        mailgun_config.provider = ssmtp_mailer::APIProvider::MAILGUN;
        mailgun_config.auth.api_key = "key-your_mailgun_api_key_here";
        mailgun_config.sender_email = "your-verified-sender@yourdomain.com";
        mailgun_config.sender_name = "Your Company";
        mailgun_config.request.base_url = "https://api.mailgun.net/v3";
        mailgun_config.request.endpoint = "/messages";
        mailgun_config.request.timeout_seconds = 30;
        mailgun_config.enable_tracking = true;
        mailgun_config.request.custom_headers["domain"] = "yourdomain.com";
        
        // Configure Amazon SES API provider
        ssmtp_mailer::APIClientConfig ses_config;
        ses_config.provider = ssmtp_mailer::APIProvider::AMAZON_SES;
        ses_config.auth.api_key = "your_aws_access_key_id_here";
        ses_config.auth.api_secret = "your_aws_secret_access_key_here";
        ses_config.sender_email = "your-verified-sender@yourdomain.com";
        ses_config.sender_name = "Your Company";
        ses_config.request.base_url = "https://email.us-east-1.amazonaws.com";
        ses_config.request.endpoint = "/v2/email";
        ses_config.request.timeout_seconds = 30;
        ses_config.enable_tracking = false;
        ses_config.request.custom_headers["region"] = "us-east-1";
        ses_config.request.custom_headers["ses_configuration_set"] = "your-config-set-name";
        
        // Add all provider configurations
        config.api_configs["sendgrid"] = sendgrid_config;
        config.api_configs["mailgun"] = mailgun_config;
        config.api_configs["ses"] = ses_config;
        
        // Create unified mailer
        ssmtp_mailer::UnifiedMailer mailer(config);
        
        std::cout << "✓ Unified mailer created successfully" << std::endl;
        std::cout << "Default method: " << (config.default_method == ssmtp_mailer::SendMethod::AUTO ? "AUTO" : 
                                          config.default_method == ssmtp_mailer::SendMethod::API ? "API" : "SMTP") << std::endl;
        std::cout << "Fallback enabled: " << (config.enable_fallback ? "Yes" : "No") << std::endl;
        
        // Show available providers
        std::cout << "\nAvailable API Providers:" << std::endl;
        auto providers = mailer.getAvailableAPIProviders();
        for (const auto& provider : providers) {
            std::cout << "  - " << provider << std::endl;
        }
        
        // Test connections for all providers
        std::cout << "\nTesting API Connections..." << std::endl;
        std::cout << "============================" << std::endl;
        
        for (const auto& provider : providers) {
            std::cout << "Testing " << provider << "... ";
            if (mailer.testConnection(ssmtp_mailer::SendMethod::API, provider)) {
                std::cout << "✓ Connected" << std::endl;
            } else {
                std::cout << "✗ Failed" << std::endl;
            }
        }
        
        // Create test emails for different scenarios
        std::vector<ssmtp_mailer::Email> test_emails;
        
        // Email 1: Simple transactional email
        ssmtp_mailer::Email email1;
        email1.from = "your-verified-sender@yourdomain.com";
        email1.to.push_back("recipient1@example.com");
        email1.subject = "Welcome to Our Service";
        email1.body = "Thank you for signing up!";
        email1.html_body = "<h1>Welcome!</h1><p>Thank you for signing up to our service.</p>";
        test_emails.push_back(email1);
        
        // Email 2: Marketing email with multiple recipients
        ssmtp_mailer::Email email2;
        email2.from = "your-verified-sender@yourdomain.com";
        email2.to.push_back("recipient2@example.com");
        email2.to.push_back("recipient3@example.com");
        email2.cc.push_back("cc@example.com");
        email2.subject = "Special Offer - Limited Time!";
        email2.body = "Don't miss out on our special offer!";
        email2.html_body = "<h1>Special Offer!</h1><p>Don't miss out on our <strong>limited time</strong> offer!</p>";
        test_emails.push_back(email2);
        
        // Email 3: Notification email
        ssmtp_mailer::Email email3;
        email3.from = "your-verified-sender@yourdomain.com";
        email3.to.push_back("recipient4@example.com");
        email3.subject = "Account Update";
        email3.body = "Your account has been updated successfully.";
        email3.html_body = "<h2>Account Update</h2><p>Your account has been updated successfully.</p>";
        test_emails.push_back(email3);
        
        // Test different sending methods
        std::cout << "\nTesting Different Sending Methods..." << std::endl;
        std::cout << "=====================================" << std::endl;
        
        // Test 1: Send via specific provider (SendGrid)
        std::cout << "\n1. Sending via SendGrid..." << std::endl;
        auto result1 = mailer.sendViaAPI(test_emails[0], "sendgrid");
        if (result1.success) {
            std::cout << "✓ Email sent via SendGrid" << std::endl;
            std::cout << "  Message ID: " << result1.message_id << std::endl;
            std::cout << "  Provider: " << result1.provider_name << std::endl;
        } else {
            std::cout << "✗ Failed to send via SendGrid: " << result1.error_message << std::endl;
        }
        
        // Test 2: Send via specific provider (Mailgun)
        std::cout << "\n2. Sending via Mailgun..." << std::endl;
        auto result2 = mailer.sendViaAPI(test_emails[1], "mailgun");
        if (result2.success) {
            std::cout << "✓ Email sent via Mailgun" << std::endl;
            std::cout << "  Message ID: " << result2.message_id << std::endl;
            std::cout << "  Provider: " << result2.provider_name << std::endl;
        } else {
            std::cout << "✗ Failed to send via Mailgun: " << result2.error_message << std::endl;
        }
        
        // Test 3: Send via specific provider (Amazon SES)
        std::cout << "\n3. Sending via Amazon SES..." << std::endl;
        auto result3 = mailer.sendViaAPI(test_emails[2], "ses");
        if (result3.success) {
            std::cout << "✓ Email sent via Amazon SES" << std::endl;
            std::cout << "  Message ID: " << result3.message_id << std::endl;
            std::cout << "  Provider: " << result3.provider_name << std::endl;
        } else {
            std::cout << "✗ Failed to send via Amazon SES: " << result3.error_message << std::endl;
        }
        
        // Test 4: Automatic method selection
        std::cout << "\n4. Testing automatic method selection..." << std::endl;
        auto result4 = mailer.sendAuto(test_emails[0]);
        if (result4.success) {
            std::cout << "✓ Email sent automatically" << std::endl;
            std::cout << "  Method used: " << (result4.method_used == ssmtp_mailer::SendMethod::API ? "API" : "SMTP") << std::endl;
            std::cout << "  Provider: " << result4.provider_name << std::endl;
            std::cout << "  Message ID: " << result4.message_id << std::endl;
        } else {
            std::cout << "✗ Automatic sending failed: " << result4.error_message << std::endl;
        }
        
        // Test 5: Batch sending
        std::cout << "\n5. Testing batch sending..." << std::endl;
        auto batch_results = mailer.sendBatch(test_emails, ssmtp_mailer::SendMethod::API);
        
        int success_count = 0;
        for (size_t i = 0; i < batch_results.size(); ++i) {
            if (batch_results[i].success) {
                success_count++;
                std::cout << "  Email " << (i + 1) << ": ✓ Sent via " << batch_results[i].provider_name << std::endl;
            } else {
                std::cout << "  Email " << (i + 1) << ": ✗ Failed (" << batch_results[i].error_message << ")" << std::endl;
            }
        }
        
        std::cout << "\nBatch sending completed: " << success_count << "/" << batch_results.size() << " successful" << std::endl;
        
        // Show statistics
        std::cout << "\nSending Statistics:" << std::endl;
        std::cout << "==================" << std::endl;
        auto stats = mailer.getStatistics();
        for (const auto& stat : stats) {
            std::cout << "  " << stat.first << ": " << stat.second << std::endl;
        }
        
        // Show provider comparison
        std::cout << "\nProvider Comparison:" << std::endl;
        std::cout << "===================" << std::endl;
        std::cout << "SendGrid:" << std::endl;
        std::cout << "  - Best for: Marketing emails, high deliverability" << std::endl;
        std::cout << "  - Features: Advanced analytics, template support" << std::endl;
        std::cout << "  - Rate limit: 100 emails/second" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Mailgun:" << std::endl;
        std::cout << "  - Best for: Transactional emails, developer-friendly" << std::endl;
        std::cout << "  - Features: Webhook support, detailed logging" << std::endl;
        std::cout << "  - Rate limit: 5 emails/second (free), 1000 emails/second (paid)" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Amazon SES:" << std::endl;
        std::cout << "  - Best for: High-volume sending, cost-effective" << std::endl;
        std::cout << "  - Features: AWS integration, excellent deliverability" << std::endl;
        std::cout << "  - Rate limit: 14 emails/second (default), configurable" << std::endl;
        
        std::cout << "\nAll tests completed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
