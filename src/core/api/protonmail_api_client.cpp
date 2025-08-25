#include "ssmtp-mailer/api_client.hpp"
#include "ssmtp-mailer/http_client.hpp"
#include "ssmtp-mailer/mailer.hpp"
#include <sstream>
#include <json/json.h>

namespace ssmtp_mailer {

ProtonMailAPIClient::ProtonMailAPIClient(const APIClientConfig& config) 
    : config_(config) {
}

APIResponse ProtonMailAPIClient::sendEmail(const Email& email) {
    APIResponse response;
    
    if (!isValid()) {
        response.error_message = "Invalid ProtonMail API client configuration";
        return response;
    }
    
    try {
        // Build request body
        std::string requestBody = buildRequestBody(email);
        std::map<std::string, std::string> headers = buildHeaders();
        
        // Make HTTP request to ProtonMail API
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
                if (root.isMember("ID")) {
                    response.message_id = root["ID"].asString();
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

std::vector<APIResponse> ProtonMailAPIClient::sendBatch(const std::vector<Email>& emails) {
    std::vector<APIResponse> responses;
    responses.reserve(emails.size());
    
    for (const auto& email : emails) {
        responses.push_back(sendEmail(email));
    }
    
    return responses;
}

bool ProtonMailAPIClient::testConnection() {
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

bool ProtonMailAPIClient::isValid() const {
    return !config_.request.base_url.empty() && 
           !config_.auth.oauth2_token.empty() &&
           !config_.sender_email.empty();
}

std::string ProtonMailAPIClient::buildRequestBody(const Email& email) {
    Json::Value root;
    
    // ProtonMail API expects specific format
    root["Subject"] = email.subject;
    root["Body"] = email.body;
    
    if (!email.html_body.empty()) {
        root["HTMLBody"] = email.html_body;
    }
    
    // From address
    Json::Value from;
    from["Address"] = config_.sender_email;
    if (!config_.sender_name.empty()) {
        from["Name"] = config_.sender_name;
    }
    root["From"] = from;
    
    // To addresses
    Json::Value toArray;
    for (const auto& recipient : email.to) {
        Json::Value to;
        to["Address"] = recipient;
        toArray.append(to);
    }
    root["To"] = toArray;
    
    // CC addresses
    if (!email.cc.empty()) {
        Json::Value ccArray;
        for (const auto& recipient : email.cc) {
            Json::Value cc;
            cc["Address"] = recipient;
            ccArray.append(cc);
        }
        root["CC"] = ccArray;
    }
    
    // BCC addresses
    if (!email.bcc.empty()) {
        Json::Value bccArray;
        for (const auto& recipient : email.bcc) {
            Json::Value bcc;
            bcc["Address"] = recipient;
            bccArray.append(bcc);
        }
        root["BCC"] = bccArray;
    }
    
    // Attachments
    if (!email.attachments.empty()) {
        Json::Value attachmentsArray;
        for (const auto& attachment_path : email.attachments) {
            Json::Value att;
            att["Filename"] = attachment_path;
            // Note: In a real implementation, you'd read the file and get content type
            att["ContentType"] = "application/octet-stream";
            attachmentsArray.append(att);
        }
        root["Attachments"] = attachmentsArray;
    }
    
    Json::FastWriter writer;
    return writer.write(root);
}

std::map<std::string, std::string> ProtonMailAPIClient::buildHeaders() {
    std::map<std::string, std::string> headers;
    
    headers["Content-Type"] = "application/json";
    headers["Accept"] = "application/json";
    headers["Authorization"] = "Bearer " + config_.auth.oauth2_token;
    
    // Add any custom headers from config
    for (const auto& header : config_.request.headers) {
        headers[header.first] = header.second;
    }
    
    return headers;
}

} // namespace ssmtp_mailer
