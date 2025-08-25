#include "core/smtp/smtp_client.hpp"
#include "ssmtp-mailer/mailer.hpp"

namespace ssmtp_mailer {

SMTPClient::SMTPClient(const ConfigManager& config) : config_(config) {
    // Stub implementation
}

SMTPClient::~SMTPClient() {
    // Stub implementation
}

SMTPResult SMTPClient::send(const Email& email) {
    (void)email; // Suppress unused parameter warning
    // Stub implementation - return success for now
    return SMTPResult::createSuccess("Email sent successfully (stub)");
}

bool SMTPClient::testConnection() {
    // Stub implementation - always return true for now
    return true;
}

} // namespace ssmtp_mailer
