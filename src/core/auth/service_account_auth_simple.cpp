#include "core/auth/service_account_auth_simple.hpp"
#include "core/logging/logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <curl/curl.h>
#include <cstring>
#include <algorithm>
#include <regex>

namespace ssmtp_mailer {

// Static callback for CURL
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

ServiceAccountAuthSimple::ServiceAccountAuthSimple(const std::string& service_account_file, 
                                                const std::string& user_email)
    : service_account_file_(service_account_file)
    , user_email_(user_email)
    , token_expiry_(0) {
    
    Logger& logger = Logger::getInstance();
    logger.info("Initializing Simple Service Account Auth for: " + user_email);
    
    if (!loadServiceAccount()) {
        throw std::runtime_error("Failed to load service account: " + service_account_file_);
    }
    
    logger.info("Simple Service Account Auth initialized successfully for: " + client_email_);
}

ServiceAccountAuthSimple::~ServiceAccountAuthSimple() {
    cleanupOpenSSL();
}

std::string ServiceAccountAuthSimple::getAccessToken() {
    if (isTokenExpired()) {
        current_token_ = generateAccessToken();
    }
    return current_token_;
}

bool ServiceAccountAuthSimple::isTokenExpired() const {
    // Consider token expired if it expires within 5 minutes
    const int buffer_time = 300; // 5 minutes
    return std::time(nullptr) + buffer_time >= token_expiry_;
}

int ServiceAccountAuthSimple::getTokenLifetime() const {
    time_t now = std::time(nullptr);
    if (token_expiry_ <= now) {
        return 0;
    }
    return static_cast<int>(token_expiry_ - now);
}

std::string ServiceAccountAuthSimple::generateAccessToken() {
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

std::string ServiceAccountAuthSimple::createJWT() {
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

std::string ServiceAccountAuthSimple::createJWTHeader() {
    return R"({"alg":"RS256","typ":"JWT"})";
}

std::string ServiceAccountAuthSimple::createJWTPayload() {
    time_t now = std::time(nullptr);
    time_t exp = now + 3600; // 1 hour from now
    
    std::ostringstream oss;
    oss << R"({"iss":")" << client_email_ << R"(","scope":")" << "https://mail.google.com/" << R"(","aud":")" 
        << token_uri_ << R"(","exp":)" << exp << R"(,"iat":)" << now << R"(,"sub":")" << user_email_ << R"("})";
    
    return oss.str();
}

std::string ServiceAccountAuthSimple::base64UrlEncode(const std::string& input) {
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

std::string ServiceAccountAuthSimple::rsaSign(const std::string& data) {
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

std::string ServiceAccountAuthSimple::exchangeJWTForToken(const std::string& jwt) {
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
    
    // Parse response using simple string parsing
    std::string access_token = extractQuotedValue(response, "access_token");
    if (access_token.empty()) {
        // Check for error
        std::string error = extractQuotedValue(response, "error");
        if (!error.empty()) {
            std::string error_desc = extractQuotedValue(response, "error_description");
            throw std::runtime_error("Token exchange failed: " + error_desc);
        }
        throw std::runtime_error("No access_token in response");
    }
    
    return access_token;
}

std::string ServiceAccountAuthSimple::extractJsonValue(const std::string& json, const std::string& key) {
    std::string pattern = "\"" + key + "\"\\s*:\\s*\"([^\"]+)\"";
    std::regex regex(pattern);
    std::smatch match;
    
    if (std::regex_search(json, match, regex) && match.size() > 1) {
        return match[1].str();
    }
    
    return "";
}

std::string ServiceAccountAuthSimple::extractQuotedValue(const std::string& json, const std::string& key) {
    return extractJsonValue(json, key);
}

bool ServiceAccountAuthSimple::loadServiceAccount() {
    std::ifstream file(service_account_file_);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open service account file: " + service_account_file_);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    std::string json_content = buffer.str();
    
    // Extract required fields using simple parsing
    std::string type = extractQuotedValue(json_content, "type");
    if (type != "service_account") {
        throw std::runtime_error("Invalid service account type: " + type);
    }
    
    std::string project_id = extractQuotedValue(json_content, "project_id");
    if (project_id.empty()) {
        throw std::runtime_error("Missing project_id in service account");
    }
    
    std::string private_key_id = extractQuotedValue(json_content, "private_key_id");
    if (private_key_id.empty()) {
        throw std::runtime_error("Missing private_key_id in service account");
    }
    
    // Extract private key (handle multi-line strings)
    std::regex private_key_regex("\"private_key\"\\s*:\\s*\"([^\"]*(?:\\\\n[^\"]*)*)\"");
    std::smatch private_key_match;
    if (std::regex_search(json_content, private_key_match, private_key_regex) && private_key_match.size() > 1) {
        private_key_ = private_key_match[1].str();
        // Replace escaped newlines
        std::string::size_type pos = 0;
        while ((pos = private_key_.find("\\n", pos)) != std::string::npos) {
            private_key_.replace(pos, 2, "\n");
            pos += 1;
        }
    } else {
        throw std::runtime_error("Missing private_key in service account");
    }
    
    client_email_ = extractQuotedValue(json_content, "client_email");
    if (client_email_.empty()) {
        throw std::runtime_error("Missing client_email in service account");
    }
    
    client_id_ = extractQuotedValue(json_content, "client_id");
    if (client_id_.empty()) {
        throw std::runtime_error("Missing client_id in service account");
    }
    
    token_uri_ = extractQuotedValue(json_content, "token_uri");
    if (token_uri_.empty()) {
        throw std::runtime_error("Missing token_uri in service account");
    }
    
    Logger& logger = Logger::getInstance();
    logger.debug("Loaded service account for project: " + project_id);
    logger.debug("Service account email: " + client_email_);
    
    return true;
}

void ServiceAccountAuthSimple::cleanupOpenSSL() {
    // OpenSSL cleanup is handled automatically in modern versions
    // This method is kept for potential future cleanup needs
}

} // namespace ssmtp_mailer
