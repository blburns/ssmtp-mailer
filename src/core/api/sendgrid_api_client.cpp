#include "ssmtp-mailer/api_client.hpp"
#include "ssmtp-mailer/http_client.hpp"
#include <sstream>
#include <iostream>
#include <algorithm>

namespace ssmtp_mailer {

SendGridAPIClient::SendGridAPIClient(const APIClientConfig& config) : config_(config) {
    // Set default SendGrid configuration if not provided
    if (config_.request.base_url.empty()) {
        config_.request.base_url = "https://api.sendgrid.com";
    }
    if (config_.request.endpoint.empty()) {
        config_.request.endpoint = "/v3/mail/send";
    }
}

APIResponse SendGridAPIClient::sendEmail(const Email& email) {
    APIResponse response;
    
    if (!isValid()) {
        response.error_message = "SendGrid client not properly configured";
        return response;
    }
    
    // Create HTTP client
    auto http_client = HTTPClientFactory::createClient();
    
    // Build request
    HTTPRequest http_request;
    http_request.method = HTTPMethod::POST;
    http_request.url = config_.request.base_url + config_.request.endpoint;
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
        // Extract message ID from response headers or body
        auto it = http_response.headers.find("X-Message-Id");
        if (it != http_response.headers.end()) {
            response.message_id = it->second;
        }
        // SendGrid doesn't always return X-Message-Id, so we might need to parse the response body
        if (response.message_id.empty() && !http_response.body.empty()) {
            // Try to extract message ID from response body (SendGrid sometimes returns it in JSON)
            // For now, we'll use a simple approach - in production you might want to parse JSON
            size_t pos = http_response.body.find("\"message_id\":");
            if (pos != std::string::npos) {
                pos += 14; // length of "\"message_id\":"
                size_t end_pos = http_response.body.find("\"", pos);
                if (end_pos != std::string::npos) {
                    response.message_id = http_response.body.substr(pos, end_pos - pos);
                }
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

std::vector<APIResponse> SendGridAPIClient::sendBatch(const std::vector<Email>& emails) {
    std::vector<APIResponse> responses;
    responses.reserve(emails.size());
    
    // SendGrid supports batch sending through their API
    // For now, we'll send them individually, but this could be optimized
    for (const auto& email : emails) {
        responses.push_back(sendEmail(email));
    }
    
    return responses;
}

bool SendGridAPIClient::testConnection() {
    // Test connection by making a simple API call
    auto http_client = HTTPClientFactory::createClient();
    
    HTTPRequest http_request;
    http_request.method = HTTPMethod::GET;
    http_request.url = config_.request.base_url + "/v3/user/profile";
    http_request.headers = buildHeaders();
    http_request.timeout_seconds = config_.request.timeout_seconds;
    http_request.verify_ssl = config_.request.verify_ssl;
    
    HTTPResponse http_response = http_client->sendRequest(http_request);
    
    // Any 2xx response means the API key is valid
    return http_response.status_code >= 200 && http_response.status_code < 300;
}

bool SendGridAPIClient::isValid() const {
    return !config_.auth.api_key.empty() && 
           !config_.request.base_url.empty() &&
           !config_.sender_email.empty();
}

std::string SendGridAPIClient::buildRequestBody(const Email& email) {
    // Build SendGrid v3 API request body
    std::ostringstream body;
    
    body << "{";
    
    // Personalizations
    body << "\"personalizations\":[{";
    body << "\"to\":[";
    for (size_t i = 0; i < email.to.size(); ++i) {
        if (i > 0) body << ",";
        body << "{\"email\":\"" << email.to[i] << "\"}";
    }
    body << "]";
    
    // CC recipients
    if (!email.cc.empty()) {
        body << ",\"cc\":[";
        for (size_t i = 0; i < email.cc.size(); ++i) {
            if (i > 0) body << ",";
            body << "{\"email\":\"" << email.cc[i] << "\"}";
        }
        body << "]";
    }
    
    // BCC recipients
    if (!email.bcc.empty()) {
        body << ",\"bcc\":[";
        for (size_t i = 0; i < email.bcc.size(); ++i) {
            if (i > 0) body << ",";
            body << "{\"email\":\"" << email.bcc[i] << "\"}";
        }
        body << "]";
    }
    
    body << "}],";
    
    // From
    body << "\"from\":{";
    body << "\"email\":\"" << email.from << "\"";
    if (!config_.sender_name.empty()) {
        body << ",\"name\":\"" << config_.sender_name << "\"";
    }
    body << "},";
    
    // Subject
    body << "\"subject\":\"" << email.subject << "\",";
    
    // Content
    body << "\"content\":[";
    
    // Plain text content
    if (!email.body.empty()) {
        body << "{\"type\":\"text/plain\",\"value\":\"" << email.body << "\"}";
    }
    
    // HTML content
    if (!email.html_body.empty()) {
        if (!email.body.empty()) body << ",";
        body << "{\"type\":\"text/html\",\"value\":\"" << email.html_body << "\"}";
    }
    
    body << "]";
    
    // Attachments (if supported)
    if (!email.attachments.empty()) {
        body << ",\"attachments\":[";
        for (size_t i = 0; i < email.attachments.size(); ++i) {
            if (i > 0) body << ",";
            // Note: This is a simplified attachment handling
            // In production, you'd want to read the file and encode it properly
            body << "{\"filename\":\"" << email.attachments[i] << "\",";
            body << "\"type\":\"application/octet-stream\"";
            body << "}";
        }
        body << "]";
    }
    
    // Tracking settings
    if (config_.enable_tracking) {
        body << ",\"tracking_settings\":{";
        body << "\"click_tracking\":{\"enable\":true,\"enable_text\":true},";
        body << "\"open_tracking\":{\"enable\":true}";
        body << "}";
    }
    
    body << "}";
    
    return body.str();
}

std::map<std::string, std::string> SendGridAPIClient::buildHeaders() {
    std::map<std::string, std::string> headers;
    
    // Authorization header
    headers["Authorization"] = "Bearer " + config_.auth.api_key;
    
    // Content type
    headers["Content-Type"] = "application/json";
    
    // User agent
    headers["User-Agent"] = "ssmtp-mailer/0.2.0";
    
    // Add custom headers from config
    for (const auto& header : config_.request.headers) {
        headers[header.first] = header.second;
    }
    
    return headers;
}

} // namespace ssmtp_mailer
