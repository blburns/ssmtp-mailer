#include "ssmtp-mailer/api_client.hpp"
#include "ssmtp-mailer/http_client.hpp"
#include <sstream>
#include <iostream>
#include <algorithm>

namespace ssmtp_mailer {

MailgunAPIClient::MailgunAPIClient(const APIClientConfig& config) : config_(config) {
    // Set default Mailgun configuration if not provided
    if (config_.request.base_url.empty()) {
        config_.request.base_url = "https://api.mailgun.net/v3";
    }
    if (config_.request.endpoint.empty()) {
        // Mailgun endpoint requires domain, so we'll set a placeholder
        config_.request.endpoint = "/messages";
    }
}

APIResponse MailgunAPIClient::sendEmail(const Email& email) {
    APIResponse response;
    
    if (!isValid()) {
        response.error_message = "Mailgun client not properly configured";
        return response;
    }
    
    // Create HTTP client
    auto http_client = HTTPClientFactory::createClient();
    
    // Build request
    HTTPRequest http_request;
    http_request.method = HTTPMethod::POST;
    
    // Mailgun requires domain in the URL
    std::string domain = getDomainFromConfig();
    if (domain.empty()) {
        response.error_message = "Mailgun domain not configured";
        return response;
    }
    
    http_request.url = config_.request.base_url + "/" + domain + "/messages";
    http_request.body = buildRequestBody(email);
    http_request.headers = buildHeaders();
    http_request.timeout_seconds = config_.request.timeout_seconds;
    http_request.verify_ssl = config_.request.verify_ssl;
    
    // Send request
    HTTPResponse http_response = http_client->sendRequest(http_request);
    
    // Process response
    response.http_code = http_response.status_code;
    response.success = http_response.success;
    response.raw_response = http_response.body;
    
    if (response.success) {
        // Extract message ID from response
        // Mailgun returns message ID in format: <20231201123456.12345.abc123@domain.com>
        response.message_id = extractMessageId(http_response.body);
        
        // If no message ID in body, try headers
        if (response.message_id.empty()) {
            auto it = http_response.headers.find("X-Mailgun-Message-Id");
            if (it != http_response.headers.end()) {
                response.message_id = it->second;
            }
        }
    } else {
        response.error_message = http_response.error_message;
        if (response.error_message.empty() && !http_response.body.empty()) {
            response.error_message = http_response.body;
        }
    }
    
    return response;
}

std::vector<APIResponse> MailgunAPIClient::sendBatch(const std::vector<Email>& emails) {
    std::vector<APIResponse> responses;
    responses.reserve(emails.size());
    
    // Mailgun supports batch sending through their API
    // For now, we'll send them individually, but this could be optimized
    for (const auto& email : emails) {
        responses.push_back(sendEmail(email));
    }
    
    return responses;
}

bool MailgunAPIClient::testConnection() {
    // Test connection by making a simple API call to get domains
    auto http_client = HTTPClientFactory::createClient();
    
    HTTPRequest http_request;
    http_request.method = HTTPMethod::GET;
    http_request.url = config_.request.base_url + "/domains";
    http_request.headers = buildHeaders();
    http_request.timeout_seconds = config_.request.timeout_seconds;
    http_request.verify_ssl = config_.request.verify_ssl;
    
    HTTPResponse http_response = http_client->sendRequest(http_request);
    
    // Any 2xx response means the API key is valid
    return http_response.status_code >= 200 && http_response.status_code < 300;
}

bool MailgunAPIClient::isValid() const {
    return !config_.auth.api_key.empty() && 
           !config_.request.base_url.empty() &&
           !config_.sender_email.empty() &&
           !getDomainFromConfig().empty();
}

std::string MailgunAPIClient::buildRequestBody(const Email& email) {
    // Mailgun uses form-encoded data, not JSON
    std::ostringstream body;
    
    // From address
    body << "from=" << urlEncode(email.from);
    
    // To addresses
    for (const auto& recipient : email.to) {
        body << "&to=" << urlEncode(recipient);
    }
    
    // CC addresses
    for (const auto& cc_recipient : email.cc) {
        body << "&cc=" << urlEncode(cc_recipient);
    }
    
    // BCC addresses
    for (const auto& bcc_recipient : email.bcc) {
        body << "&bcc=" << urlEncode(bcc_recipient);
    }
    
    // Subject
    body << "&subject=" << urlEncode(email.subject);
    
    // Body content
    if (!email.body.empty()) {
        body << "&text=" << urlEncode(email.body);
    }
    
    if (!email.html_body.empty()) {
        body << "&html=" << urlEncode(email.html_body);
    }
    
    // Sender name (if configured)
    if (!config_.sender_name.empty()) {
        std::string from_with_name = config_.sender_name + " <" + email.from + ">";
        body << "&from=" << urlEncode(from_with_name);
    }
    
    // Tracking settings
    if (config_.enable_tracking) {
        body << "&o:tracking=yes";
        body << "&o:tracking-opens=yes";
        body << "&o:tracking-clicks=yes";
    }
    
    // Custom headers (if any)
    for (const auto& header : config_.request.custom_headers) {
        body << "&h:" << header.first << "=" << urlEncode(header.second);
    }
    
    // Tags for analytics (optional)
    body << "&o:tag=ssmtp-mailer";
    
    return body.str();
}

std::map<std::string, std::string> MailgunAPIClient::buildHeaders() {
    std::map<std::string, std::string> headers;
    
    // Mailgun uses Basic Authentication with API key as username
    // The password field is not used, but some HTTP clients require it
    std::string auth_header = "Basic " + base64Encode("api:" + config_.auth.api_key);
    headers["Authorization"] = auth_header;
    
    // Content type for form data
    headers["Content-Type"] = "application/x-www-form-urlencoded";
    
    // User agent
    headers["User-Agent"] = "ssmtp-mailer/0.2.0";
    
    // Add custom headers from config
    for (const auto& header : config_.request.headers) {
        headers[header.first] = header.second;
    }
    
    return headers;
}

std::string MailgunAPIClient::getDomainFromConfig() const {
    // Try to get domain from custom headers first
    auto it = config_.request.custom_headers.find("domain");
    if (it != config_.request.custom_headers.end()) {
        return it->second;
    }
    
    // Try to extract domain from sender email
    size_t at_pos = config_.sender_email.find('@');
    if (at_pos != std::string::npos) {
        return config_.sender_email.substr(at_pos + 1);
    }
    
    return "";
}

std::string MailgunAPIClient::extractMessageId(const std::string& response_body) {
    // Mailgun response format: {"id":"<20231201123456.12345.abc123@domain.com>","message":"Queued. Thank you."}
    
    // Look for "id" field in the response
    size_t id_pos = response_body.find("\"id\":\"");
    if (id_pos != std::string::npos) {
        id_pos += 7; // length of "\"id\":\""
        size_t end_pos = response_body.find("\"", id_pos);
        if (end_pos != std::string::npos) {
            return response_body.substr(id_pos, end_pos - id_pos);
        }
    }
    
    return "";
}

std::string MailgunAPIClient::urlEncode(const std::string& str) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;
    
    for (char c : str) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
        } else {
            escaped << '%' << std::setw(2) << int((unsigned char)c);
        }
    }
    
    return escaped.str();
}

std::string MailgunAPIClient::base64Encode(const std::string& str) {
    // Simple base64 encoding for Basic Auth
    // In production, you might want to use a proper base64 library
    static const std::string base64_chars = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";
    
    std::string result;
    int val = 0, valb = -6;
    
    for (unsigned char c : str) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            result.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    
    if (valb > -6) {
        result.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    }
    
    while (result.size() % 4) {
        result.push_back('=');
    }
    
    return result;
}

} // namespace ssmtp_mailer
