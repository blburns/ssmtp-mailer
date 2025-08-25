#include <iostream>
#include <string>
#include "ssmtp-mailer/api_client.hpp"
#include "ssmtp-mailer/http_client.hpp"

int main() {
    std::cout << "ssmtp-mailer Mailgun API Example" << std::endl;
    std::cout << "=================================" << std::endl;
    
    try {
        // Create Mailgun API configuration
        ssmtp_mailer::APIClientConfig config;
        config.provider = ssmtp_mailer::APIProvider::MAILGUN;
        config.auth.api_key = "key-your_mailgun_api_key_here";
        config.sender_email = "your-verified-sender@yourdomain.com";
        config.sender_name = "Your Company";
        config.request.base_url = "https://api.mailgun.net/v3";
        config.request.endpoint = "/messages";
        config.request.timeout_seconds = 30;
        config.request.verify_ssl = true;
        config.enable_tracking = true;
        
        // Add domain to custom headers (required for Mailgun)
        config.request.custom_headers["domain"] = "yourdomain.com";
        
        // Create Mailgun API client
        auto client = ssmtp_mailer::APIClientFactory::createClient(config);
        
        if (!client->isValid()) {
            std::cerr << "Mailgun client configuration is invalid!" << std::endl;
            return 1;
        }
        
        std::cout << "✓ Mailgun client created successfully" << std::endl;
        std::cout << "Provider: " << client->getProviderName() << std::endl;
        
        // Test connection
        std::cout << "\nTesting connection..." << std::endl;
        if (client->testConnection()) {
            std::cout << "✓ Connection test successful" << std::endl;
        } else {
            std::cout << "✗ Connection test failed" << std::endl;
            std::cout << "Note: This might be due to invalid API key or domain" << std::endl;
        }
        
        // Create test email
        ssmtp_mailer::Email email;
        email.from = "your-verified-sender@yourdomain.com";
        email.to.push_back("recipient@example.com");
        email.cc.push_back("cc@example.com");
        email.bcc.push_back("bcc@example.com");
        email.subject = "Test Email from Mailgun API";
        email.body = "This is a test email sent via the Mailgun API using ssmtp-mailer.";
        email.html_body = "<h1>Test Email from Mailgun API</h1>"
                         "<p>This is a test email sent via the <strong>Mailgun API</strong> using ssmtp-mailer.</p>"
                         "<p>Features demonstrated:</p>"
                         "<ul>"
                         "<li>Multiple recipients (To, CC, BCC)</li>"
                         "<li>Both text and HTML content</li>"
                         "<li>Email tracking enabled</li>"
                         "<li>Custom headers support</li>"
                         "</ul>";
        
        std::cout << "\nSending email..." << std::endl;
        std::cout << "From: " << email.from << std::endl;
        std::cout << "To: " << email.to[0] << std::endl;
        std::cout << "CC: " << email.cc[0] << std::endl;
        std::cout << "BCC: " << email.bcc[0] << std::endl;
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
        
        // Create multiple test emails
        for (int i = 1; i <= 3; ++i) {
            ssmtp_mailer::Email batch_email;
            batch_email.from = "your-verified-sender@yourdomain.com";
            batch_email.to.push_back("batch" + std::to_string(i) + "@example.com");
            batch_email.subject = "Batch Email " + std::to_string(i);
            batch_email.body = "This is batch email number " + std::to_string(i);
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
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
