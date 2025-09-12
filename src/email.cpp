#include "simple-smtp-mailer/mailer.hpp"
#include "utils/email.hpp"
#include <algorithm>
#include <sstream>
#include <regex>

namespace ssmtp_mailer {

// Email constructors
Email::Email(const std::string& from_addr, 
             const std::string& to_addr, 
             const std::string& subject_line, 
             const std::string& body_text)
    : from(from_addr), subject(subject_line), body(body_text) {
    if (!to_addr.empty()) {
        to.push_back(to_addr);
    }
}

Email::Email(const std::string& from_addr, 
             const std::vector<std::string>& to_addrs, 
             const std::string& subject_line, 
             const std::string& body_text)
    : from(from_addr), to(to_addrs), subject(subject_line), body(body_text) {}

// Email methods
bool Email::isValid() const {
    if (from.empty() || to.empty() || subject.empty()) {
        return false;
    }
    
    if (!isValidEmailAddress(from)) {
        return false;
    }
    
    for (const auto& addr : to) {
        if (!isValidEmailAddress(addr)) {
            return false;
        }
    }
    
    for (const auto& addr : cc) {
        if (!isValidEmailAddress(addr)) {
            return false;
        }
    }
    
    for (const auto& addr : bcc) {
        if (!isValidEmailAddress(addr)) {
            return false;
        }
    }
    
    return true;
}

void Email::clear() {
    from.clear();
    to.clear();
    cc.clear();
    bcc.clear();
    subject.clear();
    body.clear();
    html_body.clear();
    attachments.clear();
}

void Email::addRecipient(const std::string& address) {
    if (isValidEmailAddress(address)) {
        to.push_back(normalizeEmailAddress(address));
    }
}

void Email::addCC(const std::string& address) {
    if (isValidEmailAddress(address)) {
        cc.push_back(normalizeEmailAddress(address));
    }
}

void Email::addBCC(const std::string& address) {
    if (isValidEmailAddress(address)) {
        bcc.push_back(normalizeEmailAddress(address));
    }
}

void Email::addAttachment(const std::string& file_path) {
    attachments.push_back(file_path);
}

bool Email::removeRecipient(const std::string& address) {
    auto it = std::find(to.begin(), to.end(), address);
    if (it != to.end()) {
        to.erase(it);
        return true;
    }
    return false;
}

bool Email::removeCC(const std::string& address) {
    auto it = std::find(cc.begin(), cc.end(), address);
    if (it != cc.end()) {
        cc.erase(it);
        return true;
    }
    return false;
}

bool Email::removeBCC(const std::string& address) {
    auto it = std::find(bcc.begin(), bcc.end(), address);
    if (it != cc.end()) {
        bcc.erase(it);
        return true;
    }
    return false;
}

bool Email::removeAttachment(const std::string& file_path) {
    auto it = std::find(attachments.begin(), attachments.end(), file_path);
    if (it != attachments.end()) {
        attachments.erase(it);
        return true;
    }
    return false;
}

std::vector<std::string> Email::getAllRecipients() const {
    std::vector<std::string> all_recipients;
    all_recipients.insert(all_recipients.end(), to.begin(), to.end());
    all_recipients.insert(all_recipients.end(), cc.begin(), cc.end());
    all_recipients.insert(all_recipients.end(), bcc.begin(), bcc.end());
    return all_recipients;
}

bool Email::isValidEmailAddress(const std::string& address) {
    return ssmtp_mailer::isValidEmailAddress(address);
}

std::string Email::extractDomain(const std::string& address) {
    return ssmtp_mailer::extractDomain(address);
}

std::string Email::extractUsername(const std::string& address) {
    return ssmtp_mailer::extractUsername(address);
}

std::string Email::normalizeEmailAddress(const std::string& address) {
    return ssmtp_mailer::normalizeEmailAddress(address);
}

bool Email::hasHtmlContent() const {
    return !html_body.empty();
}

bool Email::hasAttachments() const {
    return !attachments.empty();
}

size_t Email::getEstimatedSize() const {
    size_t size = 0;
    
    // Headers
    size += from.length() + subject.length();
    for (const auto& addr : to) size += addr.length();
    for (const auto& addr : cc) size += addr.length();
    for (const auto& addr : bcc) size += addr.length();
    
    // Body
    size += body.length() + html_body.length();
    
    // Attachments (rough estimate)
    for (const auto& attachment : attachments) {
        size += attachment.length() + 1000; // Assume 1KB per attachment
    }
    
    return size;
}

std::string Email::generateMessageId() const {
    std::string domain = extractDomain(from);
    if (domain.empty()) {
        domain = "localhost";
    }
    
    return "<" + ssmtp_mailer::generateUniqueId() + "@" + domain + ">";
}

std::string Email::getCurrentTimestamp() {
    return ssmtp_mailer::getCurrentTimestamp();
}

std::string Email::toRFC2822() const {
    std::ostringstream oss;
    
    // Date
    oss << "Date: " << getCurrentTimestamp() << "\r\n";
    
    // From
    oss << "From: " << from << "\r\n";
    
    // To
    if (!to.empty()) {
        oss << "To: ";
        for (size_t i = 0; i < to.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << to[i];
        }
        oss << "\r\n";
    }
    
    // CC
    if (!cc.empty()) {
        oss << "Cc: ";
        for (size_t i = 0; i < cc.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << cc[i];
        }
        oss << "\r\n";
    }
    
    // Subject
    oss << "Subject: " << subject << "\r\n";
    
    // Message-ID
    oss << "Message-ID: " << generateMessageId() << "\r\n";
    
    // MIME-Version
    if (hasHtmlContent() || hasAttachments()) {
        oss << "MIME-Version: 1.0\r\n";
    }
    
    // Content-Type
    if (hasHtmlContent() && hasAttachments()) {
        oss << "Content-Type: multipart/mixed; boundary=\"" << ssmtp_mailer::generateUniqueId() << "\"\r\n";
    } else if (hasHtmlContent()) {
        oss << "Content-Type: multipart/alternative; boundary=\"" << ssmtp_mailer::generateUniqueId() << "\"\r\n";
    } else if (hasAttachments()) {
        oss << "Content-Type: multipart/mixed; boundary=\"" << ssmtp_mailer::generateUniqueId() << "\"\r\n";
    } else {
        oss << "Content-Type: text/plain; charset=UTF-8\r\n";
    }
    
    oss << "\r\n";
    
    // Body
    if (hasHtmlContent() || hasAttachments()) {
        // TODO: Implement MIME multipart formatting
        oss << body;
    } else {
        oss << body;
    }
    
    return oss.str();
}

std::string Email::toMIME() const {
    // For now, return RFC2822 format
    // TODO: Implement proper MIME multipart formatting
    return toRFC2822();
}

// SMTPResult static methods
SMTPResult SMTPResult::createSuccess(const std::string& msg_id) {
    SMTPResult result;
    result.success = true;
    result.message_id = msg_id;
    return result;
}

SMTPResult SMTPResult::createError(const std::string& error_msg, int error_code) {
    SMTPResult result;
    result.success = false;
    result.error_message = error_msg;
    result.error_code = error_code;
    return result;
}

} // namespace ssmtp_mailer
