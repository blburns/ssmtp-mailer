#include "ssmtp-mailer/api_client.hpp"
#include "ssmtp-mailer/http_client.hpp"
#include "ssmtp-mailer/mailer.hpp"
#include <sstream>
#include <json/json.h>

namespace ssmtp_mailer {

ZohoMailAPIClient::ZohoMailAPIClient(const APIClientConfig& config) 
    : config_(config) {
}

APIResponse ZohoMailAPIClient::sendEmail(const Email& email) {
    APIResponse response;
    
    if (!isValid()) {
        response.error_message = "Invalid Zoho Mail API client configuration";
        return response;
    }
    
    try {
        // Build request body
        std::string requestBody = buildRequestBody(email);
        std::map<std::string, std::string> headers = buildHeaders();
        
        // Make HTTP request to Zoho Mail API
        auto httpClient = HTTPClientFactory::createClient();
        httpClient->setTimeout(config_.request.timeout_seconds);
        httpClient->setSSLVerification(config_.request.verify_ssl);
        
        HTTPRequest request;
        request.method = HTTPMethod::POST;
        request.url = config_.request.base_url + "/api/v1/messages";
        request.headers = headers;
        request.body = requestBody;
        
        HTTPResponse httpResponse = httpClient->sendRequest(request);
        
        response.http_code = httpResponse.status_code;
        response.raw_response = httpResponse.body;
        
        if (httpResponse.status_code >= 200 && httpResponse.status_code < 300) {
            response.success = true;
            
            // Parse response to extract message ID
            Json::Value root;
            Json::Reader reader;
            if (reader.parse(httpResponse.body, root)) {
                if (root.isMember("messageId")) {
                    response.message_id = root["messageId"].asString();
                } else if (root.isMember("id")) {
                    response.message_id = root["id"].asString();
                }
            }
        } else {
            response.success = false;
            response.error_message = "HTTP " + std::to_string(httpResponse.status_code) + ": " + httpResponse.body;
        }
        
    } catch (const std::exception& e) {
        response.success = false;
        response.error_message = "Exception: " + std::string(e.what());
    }
    
    return response;
}

std::vector<APIResponse> ZohoMailAPIClient::sendBatch(const std::vector<Email>& emails) {
    std::vector<APIResponse> responses;
    responses.reserve(emails.size());
    
    for (const auto& email : emails) {
        responses.push_back(sendEmail(email));
    }
    
    return responses;
}

bool ZohoMailAPIClient::testConnection() {
    try {
        auto httpClient = HTTPClientFactory::createClient();
        httpClient->setTimeout(config_.request.timeout_seconds);
        httpClient->setSSLVerification(config_.request.verify_ssl);
        
        std::map<std::string, std::string> headers = buildHeaders();
        
        HTTPRequest request;
        request.method = HTTPMethod::GET;
        request.url = config_.request.base_url + "/api/v1/status";
        request.headers = headers;
        
        HTTPResponse response = httpClient->sendRequest(request);
        
        return response.status_code >= 200 && response.status_code < 300;
    } catch (...) {
        return false;
    }
}

bool ZohoMailAPIClient::isValid() const {
    return !config_.request.base_url.empty() && 
           !config_.auth.oauth2_token.empty() &&
           !config_.sender_email.empty();
}

std::string ZohoMailAPIClient::buildRequestBody(const Email& email) {
    Json::Value root;
    
    // Zoho Mail API expects specific format
    root["subject"] = email.subject;
    root["content"] = email.body;
    
    if (!email.html_body.empty()) {
        root["htmlContent"] = email.html_body;
    }
    
    // From address
    Json::Value from;
    from["email"] = config_.sender_email;
    if (!config_.sender_name.empty()) {
        from["name"] = config_.sender_name;
    }
    root["from"] = from;
    
    // To addresses
    Json::Value toArray;
    for (const auto& recipient : email.to) {
        Json::Value to;
        to["email"] = recipient;
        toArray.append(to);
    }
    root["to"] = toArray;
    
    // CC addresses
    if (!email.cc.empty()) {
        Json::Value ccArray;
        for (const auto& recipient : email.cc) {
            Json::Value cc;
            cc["email"] = recipient;
            ccArray.append(cc);
        }
        root["cc"] = ccArray;
    }
    
    // BCC addresses
    if (!email.bcc.empty()) {
        Json::Value bccArray;
        for (const auto& recipient : email.bcc) {
            Json::Value bcc;
            bcc["email"] = recipient;
            bccArray.append(bcc);
        }
        root["bcc"] = bccArray;
    }
    
    // Attachments
    if (!email.attachments.empty()) {
        Json::Value attachmentsArray;
        for (const auto& attachment_path : email.attachments) {
            Json::Value att;
            att["filename"] = attachment_path;
            // Note: In a real implementation, you'd read the file and get content type
            att["contentType"] = "application/octet-stream";
            attachmentsArray.append(att);
        }
        root["attachments"] = attachmentsArray;
    }
    
    // Zoho Mail specific options
    if (config_.enable_tracking) {
        root["trackOpens"] = true;
        root["trackClicks"] = true;
    }
    
    Json::FastWriter writer;
    return writer.write(root);
}

std::map<std::string, std::string> ZohoMailAPIClient::buildHeaders() {
    std::map<std::string, std::string> headers;
    
    headers["Content-Type"] = "application/json";
    headers["Accept"] = "application/json";
    headers["Authorization"] = "Zoho-oauthtoken " + config_.auth.oauth2_token;
    
    // Add any custom headers from config
    for (const auto& header : config_.request.headers) {
        headers[header.first] = header.second;
    }
    
    return headers;
}

} // namespace ssmtp_mailer
