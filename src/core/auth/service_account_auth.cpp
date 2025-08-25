#include "core/auth/service_account_auth.hpp"
#include "core/logging/logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <curl/curl.h>
#include <json/json.h>
#include <cstring>
#include <algorithm>

namespace ssmtp_mailer {

// Static callback for CURL
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

ServiceAccountAuth::ServiceAccountAuth(const std::string& service_account_file, 
                                     const std::string& user_email)
    : service_account_file_(service_account_file)
    , user_email_(user_email)
    , token_expiry_(0) {
    
    Logger& logger = Logger::getInstance();
    logger.info("Initializing Service Account Auth for: " + user_email);
    
    if (!loadServiceAccount()) {
        throw std::runtime_error("Failed to load service account: " + service_account_file_);
    }
    
    logger.info("Service Account Auth initialized successfully for: " + client_email_);
}

ServiceAccountAuth::~ServiceAccountAuth() {
    cleanupOpenSSL();
}

std::string ServiceAccountAuth::getAccessToken() {
    if (isTokenExpired()) {
        current_token_ = generateAccessToken();
    }
    return current_token_;
}

bool ServiceAccountAuth::isTokenExpired() const {
    // Consider token expired if it expires within 5 minutes
    const int buffer_time = 300; // 5 minutes
    return std::time(nullptr) + buffer_time >= token_expiry_;
}

int ServiceAccountAuth::getTokenLifetime() const {
    time_t now = std::time(nullptr);
    if (token_expiry_ <= now) {
        return 0;
    }
    return static_cast<int>(token_expiry_ - now);
}

std::string ServiceAccountAuth::generateAccessToken() {
    Logger& logger = Logger::getInstance();
    logger.debug("Generating new access token for service account: " + client_email_);
    
    try {
        // Create JWT token
        std::string jwt = createJWT();
        
        // Exchange JWT for access token
        std::string access_token = exchangeJWTForToken(jwt);
        
        if (access_token.empty()) {
            throw std::runtime_error("Failed to exchange JWT for access token");
        }
        
        // Set token expiry (1 hour from now)
        token_expiry_ = std::time(nullptr) + 3600;
        
        logger.debug("Generated new access token, expires in 1 hour");
        return access_token;
        
    } catch (const std::exception& e) {
        logger.error("Failed to generate access token: " + std::string(e.what()));
        throw;
    }
}

std::string ServiceAccountAuth::createJWT() {
    std::string header = createJWTHeader();
    std::string payload = createJWTPayload();
    
    // Encode header and payload
    std::string header_encoded = base64UrlEncode(header);
    std::string payload_encoded = base64UrlEncode(payload);
    
    // Create signature
    std::string data_to_sign = header_encoded + "." + payload_encoded;
    std::string signature = rsaSign(data_to_sign);
    std::string signature_encoded = base64UrlEncode(signature);
    
    // Combine: header.payload.signature
    return data_to_sign + "." + signature_encoded;
}

std::string ServiceAccountAuth::createJWTHeader() {
    Json::Value header;
    header["alg"] = "RS256";
    header["typ"] = "JWT";
    return header.toStyledString();
}

std::string ServiceAccountAuth::createJWTPayload() {
    Json::Value payload;
    payload["iss"] = client_email_;
    payload["scope"] = "https://mail.google.com/";
    payload["aud"] = token_uri_;
    payload["exp"] = Json::Value::Int64(std::time(nullptr) + 3600);
    payload["iat"] = Json::Value::Int64(std::time(nullptr));
    payload["sub"] = user_email_;
    return payload.toStyledString();
}

std::string ServiceAccountAuth::base64UrlEncode(const std::string& input) {
    // Convert to base64
    BIO* bio = BIO_new(BIO_s_mem());
    BIO* b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);
    
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, input.c_str(), static_cast<int>(input.length()));
    BIO_flush(bio);
    
    BUF_MEM* bufferPtr;
    BIO_get_mem_ptr(bio, &bufferPtr);
    
    std::string result(bufferPtr->data, bufferPtr->length);
    
    // Convert to URL-safe base64
    std::replace(result.begin(), result.end(), '+', '-');
    std::replace(result.begin(), result.end(), '/', '_');
    
    // Remove padding
    result.erase(std::remove(result.end() - 2, result.end(), '='), result.end());
    
    BIO_free_all(bio);
    return result;
}

std::string ServiceAccountAuth::rsaSign(const std::string& data) {
    BIO* bio = BIO_new_mem_buf(private_key_.c_str(), static_cast<int>(private_key_.length()));
    if (!bio) {
        throw std::runtime_error("Failed to create BIO for private key");
    }
    
    EVP_PKEY* pkey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);
    
    if (!pkey) {
        throw std::runtime_error("Failed to read private key");
    }
    
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) {
        EVP_PKEY_free(pkey);
        throw std::runtime_error("Failed to create MD context");
    }
    
    if (EVP_DigestSignInit(ctx, nullptr, EVP_sha256(), nullptr, pkey) != 1) {
        EVP_MD_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        throw std::runtime_error("Failed to initialize signing");
    }
    
    size_t sig_len;
    if (EVP_DigestSign(ctx, nullptr, &sig_len, 
                       reinterpret_cast<const unsigned char*>(data.c_str()), data.length()) != 1) {
        EVP_MD_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        throw std::runtime_error("Failed to calculate signature length");
    }
    
    std::vector<unsigned char> signature(sig_len);
    if (EVP_DigestSign(ctx, signature.data(), &sig_len,
                       reinterpret_cast<const unsigned char*>(data.c_str()), data.length()) != 1) {
        EVP_MD_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        throw std::runtime_error("Failed to create signature");
    }
    
    EVP_MD_CTX_free(ctx);
    EVP_PKEY_free(pkey);
    
    return std::string(reinterpret_cast<char*>(signature.data()), sig_len);
}

std::string ServiceAccountAuth::exchangeJWTForToken(const std::string& jwt) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL");
    }
    
    std::string post_data = "grant_type=urn:ietf:params:oauth:grant-type:jwt-bearer&assertion=" + jwt;
    
    std::string response;
    long http_code = 0;
    
    curl_easy_setopt(curl, CURLOPT_URL, token_uri_.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    
    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    curl_easy_cleanup(curl);
    
    if (res != CURLE_OK) {
        throw std::runtime_error("CURL request failed: " + std::string(curl_easy_strerror(res)));
    }
    
    if (http_code != 200) {
        throw std::runtime_error("Token exchange failed with HTTP code: " + std::to_string(http_code));
    }
    
    // Parse response
    Json::Value json_response;
    Json::Reader reader;
    if (!reader.parse(response, json_response)) {
        throw std::runtime_error("Failed to parse token response");
    }
    
    if (json_response.isMember("error")) {
        std::string error_desc = json_response.isMember("error_description") ? 
                                json_response["error_description"].asString() : "Unknown error";
        throw std::runtime_error("Token exchange failed: " + error_desc);
    }
    
    if (!json_response.isMember("access_token")) {
        throw std::runtime_error("No access_token in response");
    }
    
    return json_response["access_token"].asString();
}

bool ServiceAccountAuth::loadServiceAccount() {
    std::ifstream file(service_account_file_);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open service account file: " + service_account_file_);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(buffer.str(), root)) {
        throw std::runtime_error("Failed to parse service account JSON");
    }
    
    // Extract required fields
    if (!root.isMember("type") || root["type"].asString() != "service_account") {
        throw std::runtime_error("Invalid service account type");
    }
    
    if (!root.isMember("project_id")) {
        throw std::runtime_error("Missing project_id in service account");
    }
    
    if (!root.isMember("private_key_id")) {
        throw std::runtime_error("Missing private_key_id in service account");
    }
    
    if (!root.isMember("private_key")) {
        throw std::runtime_error("Missing private_key in service account");
    }
    
    if (!root.isMember("client_email")) {
        throw std::runtime_error("Missing client_email in service account");
    }
    
    if (!root.isMember("client_id")) {
        throw std::runtime_error("Missing client_id in service account");
    }
    
    if (!root.isMember("token_uri")) {
        throw std::runtime_error("Missing token_uri in service account");
    }
    
    // Store the values
    private_key_ = root["private_key"].asString();
    client_email_ = root["client_email"].asString();
    client_id_ = root["client_id"].asString();
    token_uri_ = root["token_uri"].asString();
    
    Logger& logger = Logger::getInstance();
    logger.debug("Loaded service account for project: " + root["project_id"].asString());
    logger.debug("Service account email: " + client_email_);
    
    return true;
}

void ServiceAccountAuth::cleanupOpenSSL() {
    // OpenSSL cleanup is handled automatically in modern versions
    // This method is kept for potential future cleanup needs
}

} // namespace ssmtp_mailer
