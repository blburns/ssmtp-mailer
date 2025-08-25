#include "ssmtp-mailer/unified_mailer.hpp"
#include "core/config/config_manager.hpp"
#include "ssmtp-mailer/smtp_client.hpp"
#include <algorithm>
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>

namespace ssmtp_mailer {

UnifiedMailer::UnifiedMailer(const UnifiedMailerConfig& config) : config_(config) {
    initializeSMTP();
    initializeAPIClients();
    
    // Initialize statistics
    stats_["smtp_success"] = 0;
    stats_["smtp_failure"] = 0;
    stats_["api_success"] = 0;
    stats_["api_failure"] = 0;
    stats_["retries"] = 0;
    stats_["fallbacks"] = 0;
}

UnifiedMailer::~UnifiedMailer() = default;

UnifiedMailerResult UnifiedMailer::sendEmail(const Email& email, SendMethod method) {
    switch (method) {
        case SendMethod::SMTP:
            return sendViaSMTP(email);
            
        case SendMethod::API:
            return sendViaAPI(email);
            
        case SendMethod::AUTO:
        default:
            return sendAuto(email);
    }
}

UnifiedMailerResult UnifiedMailer::sendViaSMTP(const Email& email) {
    UnifiedMailerResult result;
    result.method_used = SendMethod::SMTP;
    
    try {
        if (!smtp_config_) {
            result.error_message = "SMTP configuration not available";
            return result;
        }
        
        // Create SMTP client and send email
        SMTPClient smtp_client(*smtp_config_);
        SMTPResult smtp_result = smtp_client.send(email);
        
        result.success = smtp_result.success;
        if (result.success) {
            result.message_id = smtp_result.message_id;
            updateStats("smtp_success", true);
        } else {
            result.error_message = smtp_result.error_message;
            updateStats("smtp_failure", true);
        }
        
    } catch (const std::exception& e) {
        result.error_message = "SMTP error: " + std::string(e.what());
        updateStats("smtp_failure", true);
    }
    
    return result;
}

UnifiedMailerResult UnifiedMailer::sendViaAPI(const Email& email, const std::string& provider) {
    UnifiedMailerResult result;
    result.method_used = SendMethod::API;
    
    try {
        std::string selected_provider = provider;
        if (selected_provider.empty()) {
            selected_provider = selectBestProvider(email);
        }
        
        if (selected_provider.empty()) {
            result.error_message = "No API provider available";
            return result;
        }
        
        auto it = api_clients_.find(selected_provider);
        if (it == api_clients_.end()) {
            result.error_message = "API provider '" + selected_provider + "' not available";
            return result;
        }
        
        // Send email via API
        APIResponse api_response = it->second->sendEmail(email);
        
        result.success = api_response.success;
        result.provider_name = selected_provider;
        
        if (result.success) {
            result.message_id = api_response.message_id;
            updateStats("api_success", true);
        } else {
            result.error_message = api_response.error_message;
            updateStats("api_failure", true);
        }
        
    } catch (const std::exception& e) {
        result.error_message = "API error: " + std::string(e.what());
        updateStats("api_failure", true);
    }
    
    return result;
}

UnifiedMailerResult UnifiedMailer::sendAuto(const Email& email) {
    UnifiedMailerResult result;
    
    // Try API first (faster, more reliable)
    result = sendViaAPI(email);
    
    // If API fails and fallback is enabled, try SMTP
    if (!result.success && config_.enable_fallback) {
        updateStats("fallbacks", true);
        result = sendViaSMTP(email);
        result.method_used = SendMethod::SMTP; // Override to show fallback was used
    }
    
    return result;
}

std::vector<UnifiedMailerResult> UnifiedMailer::sendBatch(const std::vector<Email>& emails, 
                                                         SendMethod method) {
    std::vector<UnifiedMailerResult> results;
    results.reserve(emails.size());
    
    for (const auto& email : emails) {
        results.push_back(sendEmail(email, method));
    }
    
    return results;
}

bool UnifiedMailer::testConnection(SendMethod method, const std::string& provider) {
    switch (method) {
        case SendMethod::SMTP:
            // Test SMTP connection
            try {
                if (!smtp_config_) return false;
                // Create a simple test email
                Email test_email("test@example.com", "test@example.com", "Test", "Test");
                SMTPClient smtp_client(*smtp_config_);
                return smtp_client.testConnection();
            } catch (...) {
                return false;
            }
            
        case SendMethod::API: {
            // Test API connection
            if (provider.empty()) {
                // Test first available provider
                auto providers = getAvailableAPIProviders();
                if (providers.empty()) return false;
                return testConnection(method, providers[0]);
            }
            
            auto it = api_clients_.find(provider);
            if (it == api_clients_.end()) return false;
            
            return it->second->testConnection();
        }
            
        case SendMethod::AUTO:
            // Test both methods
            return testConnection(SendMethod::SMTP) || testConnection(SendMethod::API);
            
        default:
            return false;
    }
}

std::vector<std::string> UnifiedMailer::getAvailableAPIProviders() const {
    std::vector<std::string> providers;
    providers.reserve(api_clients_.size());
    
    for (const auto& pair : api_clients_) {
        if (pair.second && pair.second->isValid()) {
            providers.push_back(pair.first);
        }
    }
    
    return providers;
}

bool UnifiedMailer::isProviderAvailable(const std::string& provider) const {
    auto it = api_clients_.find(provider);
    return it != api_clients_.end() && it->second && it->second->isValid();
}

void UnifiedMailer::setDefaultMethod(SendMethod method) {
    config_.default_method = method;
}

void UnifiedMailer::setAPIConfig(const std::string& provider, const APIClientConfig& config) {
    config_.api_configs[provider] = config;
    
    // Recreate the API client
    try {
        auto client = APIClientFactory::createClient(config);
        api_clients_[provider] = client;
    } catch (const std::exception& e) {
        std::cerr << "Failed to create API client for " << provider << ": " << e.what() << std::endl;
    }
}

void UnifiedMailer::removeAPIConfig(const std::string& provider) {
    config_.api_configs.erase(provider);
    api_clients_.erase(provider);
}

std::map<std::string, size_t> UnifiedMailer::getStatistics() const {
    std::lock_guard<std::mutex> lock(stats_mutex_);
    return stats_;
}

// Private helper methods

void UnifiedMailer::initializeSMTP() {
    if (!config_.smtp_config_file.empty()) {
        try {
            smtp_config_ = std::make_unique<ConfigManager>();
            smtp_config_->loadFromFile(config_.smtp_config_file);
        } catch (const std::exception& e) {
            std::cerr << "Failed to initialize SMTP configuration: " << e.what() << std::endl;
        }
    }
}

void UnifiedMailer::initializeAPIClients() {
    for (const auto& pair : config_.api_configs) {
        try {
            auto client = APIClientFactory::createClient(pair.second);
            api_clients_[pair.first] = client;
        } catch (const std::exception& e) {
            std::cerr << "Failed to initialize API client for " << pair.first << ": " << e.what() << std::endl;
        }
    }
}

void UnifiedMailer::updateStats(const std::string& key, bool success) {
    (void)success; // Suppress unused parameter warning
    std::lock_guard<std::mutex> lock(stats_mutex_);
    auto it = stats_.find(key);
    if (it != stats_.end()) {
        it->second++;
    }
}

std::string UnifiedMailer::selectBestProvider(const Email& email) {
    (void)email; // Suppress unused parameter warning
    // Simple provider selection logic
    // In a real implementation, you might want to consider:
    // - Provider reliability
    // - Cost
    // - Geographic location
    // - Rate limits
    // - Email type (transactional vs marketing)
    
    auto providers = getAvailableAPIProviders();
    if (providers.empty()) return "";
    
    // For now, just return the first available provider
    // You could implement more sophisticated selection logic here
    return providers[0];
}

bool UnifiedMailer::shouldRetry(const UnifiedMailerResult& result) {
    if (result.success) return false;
    if (result.retry_count >= config_.max_retries) return false;
    
    // Retry on certain error types
    const std::string& error = result.error_message;
    if (error.find("timeout") != std::string::npos ||
        error.find("connection") != std::string::npos ||
        error.find("rate limit") != std::string::npos ||
        error.find("temporary") != std::string::npos) {
        return true;
    }
    
    return false;
}

UnifiedMailerResult UnifiedMailer::retryWithFallback(const Email& email, SendMethod original_method) {
    UnifiedMailerResult result;
    result.retry_count = 1;
    
    // Wait before retry
    std::this_thread::sleep_for(config_.retry_delay);
    
    // Try the other method
    if (original_method == SendMethod::SMTP) {
        result = sendViaAPI(email);
        result.method_used = SendMethod::API;
    } else {
        result = sendViaSMTP(email);
        result.method_used = SendMethod::SMTP;
    }
    
    updateStats("retries", true);
    
    return result;
}

} // namespace ssmtp_mailer
