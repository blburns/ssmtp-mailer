#include "ssmtp-mailer/api_client.hpp"
#include <memory>
#include <stdexcept>

namespace ssmtp_mailer {

std::shared_ptr<BaseAPIClient> APIClientFactory::createClient(const APIClientConfig& config) {
    switch (config.provider) {
        case APIProvider::SENDGRID:
            return std::make_shared<SendGridAPIClient>(config);
            
        case APIProvider::MAILGUN:
            return std::make_shared<MailgunAPIClient>(config);
            
        case APIProvider::AMAZON_SES:
            return std::make_shared<AmazonSESAPIClient>(config);
            
        case APIProvider::PROTONMAIL:
            return std::make_shared<ProtonMailAPIClient>(config);
            
        case APIProvider::ZOHO_MAIL:
            return std::make_shared<ZohoMailAPIClient>(config);
            
        case APIProvider::FASTMAIL:
            return std::make_shared<FastmailAPIClient>(config);
            
        case APIProvider::POSTMARK:
        case APIProvider::SPARKPOST:
        case APIProvider::MAILJET:
            // These providers are not yet implemented
            throw std::runtime_error("Provider " + std::to_string(static_cast<int>(config.provider)) + " is not yet implemented");
            
        case APIProvider::CUSTOM:
            // Custom provider would require custom implementation
            throw std::runtime_error("Custom providers require custom implementation");
            
        default:
            throw std::runtime_error("Unknown API provider: " + std::to_string(static_cast<int>(config.provider)));
    }
}

std::vector<std::string> APIClientFactory::getSupportedProviders() {
    return {
        "SendGrid",
        "Mailgun", 
        "Amazon SES",
        "ProtonMail",
        "Zoho Mail",
        "Fastmail"
        // Add more providers as they are implemented
    };
}

bool APIClientFactory::isProviderSupported(APIProvider provider) {
    switch (provider) {
        case APIProvider::SENDGRID:
        case APIProvider::MAILGUN:
        case APIProvider::AMAZON_SES:
        case APIProvider::PROTONMAIL:
        case APIProvider::ZOHO_MAIL:
        case APIProvider::FASTMAIL:
            return true;
            
        case APIProvider::POSTMARK:
        case APIProvider::SPARKPOST:
        case APIProvider::MAILJET:
        case APIProvider::CUSTOM:
        default:
            return false;
    }
}

} // namespace ssmtp_mailer
