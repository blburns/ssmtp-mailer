#include <iostream>
#include <string>
#include "ssmtp-mailer/api_client.hpp"
#include "ssmtp-mailer/http_client.hpp"

int main() {
    std::cout << "ssmtp-mailer Amazon SES API Example" << std::endl;
    std::cout << "====================================" << std::endl;
    
    try {
        // Create Amazon SES API configuration
        ssmtp_mailer::APIClientConfig config;
        config.provider = ssmtp_mailer::APIProvider::AMAZON_SES;
        config.auth.api_key = "your_aws_access_key_id_here";
        config.auth.api_secret = "your_aws_secret_access_key_here";
        config.sender_email = "your-verified-sender@yourdomain.com";
        config.sender_name = "Your Company";
        config.request.base_url = "https://email.us-east-1.amazonaws.com";
        config.request.endpoint = "/v2/email";
        config.request.timeout_seconds = 30;
        config.request.verify_ssl = true;
        config.enable_tracking = false; // SES handles tracking differently
        
        // Add AWS-specific configuration to custom headers
        config.request.custom_headers["region"] = "us-east-1";
        config.request.custom_headers["ses_configuration_set"] = "your-config-set-name";
        
        // Create Amazon SES API client
        auto client = ssmtp_mailer::APIClientFactory::createClient(config);
        
        if (!client->isValid()) {
            std::cerr << "Amazon SES client configuration is invalid!" << std::endl;
            return 1;
        }
        
        std::cout << "✓ Amazon SES client created successfully" << std::endl;
        std::cout << "Provider: " << client->getProviderName() << std::endl;
        
        // Test connection
        std::cout << "\nTesting connection..." << std::endl;
        if (client->testConnection()) {
            std::cout << "✓ Connection test successful" << std::endl;
        } else {
            std::cout << "✗ Connection test failed" << std::endl;
            std::cout << "Note: This might be due to invalid AWS credentials or region" << std::endl;
        }
        
        // Create test email
        ssmtp_mailer::Email email;
        email.from = "your-verified-sender@yourdomain.com";
        email.to.push_back("recipient@example.com");
        email.to.push_back("recipient2@example.com");
        email.cc.push_back("cc@example.com");
        email.bcc.push_back("bcc@example.com");
        email.subject = "Test Email from Amazon SES API";
        email.body = "This is a test email sent via the Amazon SES API using ssmtp-mailer.";
        email.html_body = "<h1>Test Email from Amazon SES API</h1>"
                         "<p>This is a test email sent via the <strong>Amazon SES API</strong> using ssmtp-mailer.</p>"
                         "<p>Features demonstrated:</p>"
                         "<ul>"
                         "<li>Multiple recipients (To, CC, BCC)</li>"
                         "<li>Both text and HTML content</li>"
                         "<li>AWS SES v2 API integration</li>"
                         "<li>Configuration set support</li>"
                         "<li>Email tagging for analytics</li>"
                         "</ul>"
                         "<p><em>Note: Amazon SES provides excellent deliverability and scalability for high-volume email sending.</em></p>";
        
        std::cout << "\nSending email..." << std::endl;
        std::cout << "From: " << email.from << std::endl;
        std::cout << "To: " << email.to.size() << " recipients" << std::endl;
        std::cout << "CC: " << email.cc.size() << " recipients" << std::endl;
        std::cout << "BCC: " << email.bcc.size() << " recipients" << std::endl;
        std::cout << "Subject: " << email.subject << std::endl;
        
        // Send email
        auto result = client->sendEmail(email);
        
        if (result.success) {
            std::cout << "\n✓ Email sent successfully!" << std::endl;
            std::cout << "Message ID: " << result.message_id << std::endl;
            std::cout << "HTTP Code: " << result.http_code << std::endl;
        } else {
            std::cout << "\n✗ Failed to send email" << std::endl;
            std::cout << "Error: " << result.error_message << std::endl;
            std::cout << "HTTP Code: " << result.http_code << std::endl;
            
            if (!result.raw_response.empty()) {
                std::cout << "Raw Response: " << result.raw_response << std::endl;
            }
        }
        
        // Test batch sending
        std::cout << "\nTesting batch sending..." << std::endl;
        std::vector<ssmtp_mailer::Email> batch_emails;
        
        // Create multiple test emails with different content
        std::vector<std::string> subjects = {
            "Welcome to Our Service",
            "Monthly Newsletter",
            "Account Update"
        };
        
        std::vector<std::string> bodies = {
            "Welcome! We're excited to have you on board.",
            "Here's what's new this month...",
            "Your account has been updated successfully."
        };
        
        for (int i = 0; i < 3; ++i) {
            ssmtp_mailer::Email batch_email;
            batch_email.from = "your-verified-sender@yourdomain.com";
            batch_email.to.push_back("batch" + std::to_string(i + 1) + "@example.com");
            batch_email.subject = subjects[i];
            batch_email.body = bodies[i];
            batch_email.html_body = "<h2>" + subjects[i] + "</h2><p>" + bodies[i] + "</p>";
            batch_emails.push_back(batch_email);
        }
        
        std::cout << "Sending " << batch_emails.size() << " emails in batch..." << std::endl;
        auto batch_results = client->sendBatch(batch_emails);
        
        int success_count = 0;
        for (size_t i = 0; i < batch_results.size(); ++i) {
            if (batch_results[i].success) {
                success_count++;
                std::cout << "  Email " << (i + 1) << ": ✓ Sent (ID: " << batch_results[i].message_id << ")" << std::endl;
            } else {
                std::cout << "  Email " << (i + 1) << ": ✗ Failed (" << batch_results[i].error_message << ")" << std::endl;
            }
        }
        
        std::cout << "\nBatch sending completed: " << success_count << "/" << batch_emails.size() << " successful" << std::endl;
        
        // Show AWS-specific information
        std::cout << "\nAWS SES Information:" << std::endl;
        std::cout << "====================" << std::endl;
        std::cout << "Region: us-east-1" << std::endl;
        std::cout << "API Version: v2" << std::endl;
        std::cout << "Configuration Set: " << config.request.custom_headers["ses_configuration_set"] << std::endl;
        std::cout << "Note: Amazon SES provides excellent deliverability and is cost-effective for high-volume sending." << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
