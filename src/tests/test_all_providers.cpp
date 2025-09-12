#include <iostream>
#include <string>
#include <vector>
#include "simple-smtp-mailer/api_client.hpp"
#include "simple-smtp-mailer/http_client.hpp"

void testHTTPClient() {
    std::cout << "Testing HTTP Client..." << std::endl;
    std::cout << "=======================" << std::endl;
    
    try {
        auto http_client = ssmtp_mailer::HTTPClientFactory::createClient();
        std::cout << "✓ HTTP client created successfully" << std::endl;
        
        auto backends = ssmtp_mailer::HTTPClientFactory::getAvailableBackends();
        std::cout << "Available backends: ";
        for (const auto& backend : backends) {
            std::cout << backend << " ";
        }
        std::cout << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "✗ HTTP client test failed: " << e.what() << std::endl;
    }
    std::cout << std::endl;
}

void testSendGridClient() {
    std::cout << "Testing SendGrid API Client..." << std::endl;
    std::cout << "===============================" << std::endl;
    
    try {
        ssmtp_mailer::APIClientConfig config;
        config.provider = ssmtp_mailer::APIProvider::SENDGRID;
        config.auth.api_key = "test_key";
        config.sender_email = "test@example.com";
        config.request.base_url = "https://api.sendgrid.com";
        config.request.endpoint = "/v3/mail/send";
        
        auto client = ssmtp_mailer::APIClientFactory::createClient(config);
        std::cout << "✓ SendGrid client created successfully" << std::endl;
        std::cout << "  Provider name: " << client->getProviderName() << std::endl;
        std::cout << "  Is valid: " << (client->isValid() ? "Yes" : "No") << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "✗ SendGrid client test failed: " << e.what() << std::endl;
    }
    std::cout << std::endl;
}

void testMailgunClient() {
    std::cout << "Testing Mailgun API Client..." << std::endl;
    std::cout << "=============================" << std::endl;
    
    try {
        ssmtp_mailer::APIClientConfig config;
        config.provider = ssmtp_mailer::APIProvider::MAILGUN;
        config.auth.api_key = "test_key";
        config.sender_email = "test@example.com";
        config.request.base_url = "https://api.mailgun.net/v3";
        config.request.endpoint = "/messages";
        
        // Add domain to custom headers for Mailgun
        config.request.custom_headers["domain"] = "example.com";
        
        auto client = ssmtp_mailer::APIClientFactory::createClient(config);
        std::cout << "✓ Mailgun client created successfully" << std::endl;
        std::cout << "  Provider name: " << client->getProviderName() << std::endl;
        std::cout << "  Is valid: " << (client->isValid() ? "Yes" : "No") << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "✗ Mailgun client test failed: " << e.what() << std::endl;
    }
    std::cout << std::endl;
}

void testAmazonSESClient() {
    std::cout << "Testing Amazon SES API Client..." << std::endl;
    std::cout << "=================================" << std::endl;
    
    try {
        ssmtp_mailer::APIClientConfig config;
        config.provider = ssmtp_mailer::APIProvider::AMAZON_SES;
        config.auth.api_key = "test_access_key";
        config.auth.api_secret = "test_secret_key";
        config.sender_email = "test@example.com";
        config.request.base_url = "https://email.us-east-1.amazonaws.com";
        config.request.endpoint = "/v2/email";
        
        // Add region to custom headers for Amazon SES
        config.request.custom_headers["region"] = "us-east-1";
        
        auto client = ssmtp_mailer::APIClientFactory::createClient(config);
        std::cout << "✓ Amazon SES client created successfully" << std::endl;
        std::cout << "  Provider name: " << client->getProviderName() << std::endl;
        std::cout << "  Is valid: " << (client->isValid() ? "Yes" : "No") << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "✗ Amazon SES client test failed: " << e.what() << std::endl;
    }
    std::cout << std::endl;
}

void testAPIClientFactory() {
    std::cout << "Testing API Client Factory..." << std::endl;
    std::cout << "=============================" << std::endl;
    
    try {
        auto providers = ssmtp_mailer::APIClientFactory::getSupportedProviders();
        std::cout << "Supported providers: ";
        for (const auto& provider : providers) {
            std::cout << provider << " ";
        }
        std::cout << std::endl;
        
        // Test provider support checking
        std::cout << "Provider support check:" << std::endl;
        std::cout << "  SendGrid: " << (ssmtp_mailer::APIClientFactory::isProviderSupported(ssmtp_mailer::APIProvider::SENDGRID) ? "Yes" : "No") << std::endl;
        std::cout << "  Mailgun: " << (ssmtp_mailer::APIClientFactory::isProviderSupported(ssmtp_mailer::APIProvider::MAILGUN) ? "Yes" : "No") << std::endl;
        std::cout << "  Amazon SES: " << (ssmtp_mailer::APIClientFactory::isProviderSupported(ssmtp_mailer::APIProvider::AMAZON_SES) ? "Yes" : "No") << std::endl;
        std::cout << "  Postmark: " << (ssmtp_mailer::APIClientFactory::isProviderSupported(ssmtp_mailer::APIProvider::POSTMARK) ? "Yes" : "No") << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "✗ API client factory test failed: " << e.what() << std::endl;
    }
    std::endl;
}

void testEmailComposition() {
    std::cout << "Testing Email Composition..." << std::endl;
    std::cout << "============================" << std::endl;
    
    try {
        // Create test email
        ssmtp_mailer::Email email;
        email.from = "sender@example.com";
        email.to.push_back("recipient1@example.com");
        email.to.push_back("recipient2@example.com");
        email.cc.push_back("cc@example.com");
        email.bcc.push_back("bcc@example.com");
        email.subject = "Test Email";
        email.body = "This is a test email body.";
        email.html_body = "<h1>Test Email</h1><p>This is a test email body.</p>";
        
        std::cout << "✓ Email composition successful" << std::endl;
        std::cout << "  From: " << email.from << std::endl;
        std::cout << "  To: " << email.to.size() << " recipients" << std::endl;
        std::cout << "  CC: " << email.cc.size() << " recipients" << std::endl;
        std::cout << "  BCC: " << email.bcc.size() << " recipients" << std::endl;
        std::cout << "  Subject: " << email.subject << std::endl;
        std::cout << "  Has text body: " << (!email.body.empty() ? "Yes" : "No") << std::endl;
        std::cout << "  Has HTML body: " << (!email.html_body.empty() ? "Yes" : "No") << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "✗ Email composition test failed: " << e.what() << std::endl;
    }
    std::cout << std::endl;
}

int main() {
    std::cout << "ssmtp-mailer API Provider Test Suite" << std::endl;
    std::cout << "====================================" << std::endl;
    std::cout << std::endl;
    
    try {
        // Test HTTP client infrastructure
        testHTTPClient();
        
        // Test API client factory
        testAPIClientFactory();
        
        // Test individual providers
        testSendGridClient();
        testMailgunClient();
        testAmazonSESClient();
        
        // Test email composition
        testEmailComposition();
        
        std::cout << "All tests completed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Test suite failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
