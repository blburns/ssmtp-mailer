#include <iostream>
#include <string>
#include "ssmtp-mailer/api_client.hpp"
#include "ssmtp-mailer/http_client.hpp"

int main() {
    std::cout << "Testing API Client Functionality" << std::endl;
    std::cout << "=================================" << std::endl;
    
    try {
        // Test HTTP client creation
        std::cout << "1. Testing HTTP client creation..." << std::endl;
        auto http_client = ssmtp_mailer::HTTPClientFactory::createClient();
        std::cout << "   ✓ HTTP client created successfully" << std::endl;
        
        // Test available backends
        std::cout << "2. Testing available backends..." << std::endl;
        auto backends = ssmtp_mailer::HTTPClientFactory::getAvailableBackends();
        std::cout << "   Available backends: ";
        for (const auto& backend : backends) {
            std::cout << backend << " ";
        }
        std::cout << std::endl;
        
        // Test API client factory
        std::cout << "3. Testing API client factory..." << std::endl;
        auto providers = ssmtp_mailer::APIClientFactory::getSupportedProviders();
        std::cout << "   Supported providers: ";
        for (const auto& provider : providers) {
            std::cout << provider << " ";
        }
        std::cout << std::endl;
        
        // Test SendGrid client creation (without valid config)
        std::cout << "4. Testing SendGrid client creation..." << std::endl;
        ssmtp_mailer::APIClientConfig config;
        config.provider = ssmtp_mailer::APIProvider::SENDGRID;
        config.auth.api_key = "test_key";
        config.sender_email = "test@example.com";
        
        try {
            auto client = ssmtp_mailer::APIClientFactory::createClient(config);
            std::cout << "   ✓ SendGrid client created successfully" << std::endl;
            std::cout << "   Provider name: " << client->getProviderName() << std::endl;
            std::cout << "   Is valid: " << (client->isValid() ? "Yes" : "No") << std::endl;
        } catch (const std::exception& e) {
            std::cout << "   ✗ Failed to create SendGrid client: " << e.what() << std::endl;
        }
        
        std::cout << "\nAll tests completed!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
