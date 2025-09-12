#include "core/smtp/smtp_client.hpp"
#include "simple-smtp-mailer/mailer.hpp"
#include "core/logging/logger.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sstream>
#include <regex>
#include <openssl/ssl.h>
#include <openssl/err.h>

namespace ssmtp_mailer {

SMTPClient::SMTPClient(const ConfigManager& config) : config_(config), socket_fd_(-1), ssl_context_(nullptr), ssl_connection_(nullptr) {
    // Initialize OpenSSL
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
}

SMTPClient::~SMTPClient() {
    disconnect();
    if (ssl_context_) {
        SSL_CTX_free(ssl_context_);
    }
}

SMTPResult SMTPClient::send(const Email& email) {
    Logger& logger = Logger::getInstance();
    
    try {
        // Get domain configuration for the from address
        std::string domain = email.from.substr(email.from.find('@') + 1);
        const DomainConfig* domain_config = config_.getDomainConfig(domain);
        
        if (!domain_config) {
            return SMTPResult::createError("No configuration found for domain: " + domain);
        }
        
        // Use curl to send email via SMTP
        return sendViaCurl(email, domain_config);
        
    } catch (const std::exception& e) {
        logger.error("SMTP send error: " + std::string(e.what()));
        return SMTPResult::createError("SMTP error: " + std::string(e.what()));
    }
}

bool SMTPClient::connect(const std::string& server, int port, bool use_ssl) {
    Logger& logger = Logger::getInstance();
    
    // Create socket
    socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd_ < 0) {
        logger.error("Failed to create socket");
        return false;
    }
    
    // Get server address
    struct hostent* server_info = gethostbyname(server.c_str());
    if (!server_info) {
        logger.error("Failed to resolve hostname: " + server);
        close(socket_fd_);
        socket_fd_ = -1;
        return false;
    }
    
    // Set up server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    memcpy(&server_addr.sin_addr, server_info->h_addr, server_info->h_length);
    
    // Connect to server
    if (::connect(socket_fd_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        logger.error("Failed to connect to SMTP server: " + server + ":" + std::to_string(port));
        close(socket_fd_);
        socket_fd_ = -1;
        return false;
    }
    
    // Read initial response
    std::string response = readResponse();
    if (response.empty() || response[0] != '2') {
        logger.error("SMTP server rejected connection: " + response);
        close(socket_fd_);
        socket_fd_ = -1;
        return false;
    }
    
    // Set up SSL if required
    if (use_ssl) {
        if (!setupSSL()) {
            disconnect();
            return false;
        }
    }
    
    logger.info("Connected to SMTP server: " + server + ":" + std::to_string(port));
    return true;
}

void SMTPClient::disconnect() {
    if (socket_fd_ >= 0) {
        if (ssl_connection_) {
            SSL_free(ssl_connection_);
            ssl_connection_ = nullptr;
        }
        close(socket_fd_);
        socket_fd_ = -1;
    }
}

bool SMTPClient::authenticate(const std::string& username, const std::string& password, SMTPAuthMethod auth_method) {
    Logger& logger = Logger::getInstance();
    
    switch (auth_method) {
        case SMTPAuthMethod::LOGIN:
            return authenticateLogin(username, password);
        case SMTPAuthMethod::PLAIN:
            return authenticatePlain(username, password);
        default:
            logger.warning("Unsupported authentication method");
            return false;
    }
}

bool SMTPClient::testConnection() {
    // This is a simple test - just try to connect and disconnect
    const DomainConfig* domain_config = nullptr;
    
    // Try to find any configured domain
    // For now, we'll just return true if we have a socket
    return socket_fd_ >= 0;
}

// Private helper methods
bool SMTPClient::setupSSL() {
    Logger& logger = Logger::getInstance();
    
    // Create SSL context
    ssl_context_ = SSL_CTX_new(TLS_client_method());
    if (!ssl_context_) {
        logger.error("Failed to create SSL context");
        return false;
    }
    
    // Create SSL connection
    ssl_connection_ = SSL_new(ssl_context_);
    if (!ssl_connection_) {
        logger.error("Failed to create SSL connection");
        return false;
    }
    
    // Set socket for SSL
    if (SSL_set_fd(ssl_connection_, socket_fd_) != 1) {
        logger.error("Failed to set SSL socket");
        return false;
    }
    
    // Perform SSL handshake
    if (SSL_connect(ssl_connection_) != 1) {
        logger.error("SSL handshake failed");
        return false;
    }
    
    logger.info("SSL connection established");
    return true;
}

std::string SMTPClient::readResponse() {
    std::string response;
    char buffer[1024];
    
    while (true) {
        int bytes_read;
        if (ssl_connection_) {
            bytes_read = SSL_read(ssl_connection_, buffer, sizeof(buffer) - 1);
        } else {
            bytes_read = recv(socket_fd_, buffer, sizeof(buffer) - 1, 0);
        }
        
        if (bytes_read <= 0) {
            break;
        }
        
        buffer[bytes_read] = '\0';
        response += buffer;
        
        // Check if response is complete (ends with \r\n)
        if (response.length() >= 2 && 
            response[response.length() - 2] == '\r' && 
            response[response.length() - 1] == '\n') {
            break;
        }
    }
    
    return response;
}

bool SMTPClient::sendCommand(const std::string& command) {
    std::string full_command = command + "\r\n";
    
    int bytes_sent;
    if (ssl_connection_) {
        bytes_sent = SSL_write(ssl_connection_, full_command.c_str(), full_command.length());
    } else {
        bytes_sent = ::send(socket_fd_, full_command.c_str(), full_command.length(), 0);
    }
    
    return bytes_sent == static_cast<int>(full_command.length());
}

bool SMTPClient::authenticateLogin(const std::string& username, const std::string& password) {
    Logger& logger = Logger::getInstance();
    
    // Send AUTH LOGIN command
    if (!sendCommand("AUTH LOGIN")) {
        return false;
    }
    
    std::string response = readResponse();
    if (response[0] != '3') {
        logger.error("AUTH LOGIN not supported: " + response);
        return false;
    }
    
    // Send username (base64 encoded)
    std::string encoded_username = base64Encode(username);
    if (!sendCommand(encoded_username)) {
        return false;
    }
    
    response = readResponse();
    if (response[0] != '3') {
        logger.error("Username rejected: " + response);
        return false;
    }
    
    // Send password (base64 encoded)
    std::string encoded_password = base64Encode(password);
    if (!sendCommand(encoded_password)) {
        return false;
    }
    
    response = readResponse();
    if (response[0] != '2') {
        logger.error("Authentication failed: " + response);
        return false;
    }
    
    logger.info("SMTP authentication successful");
    return true;
}

bool SMTPClient::authenticatePlain(const std::string& username, const std::string& password) {
    Logger& logger = Logger::getInstance();
    
    // Create PLAIN authentication string
    std::string auth_string = "\0" + username + "\0" + password;
    std::string encoded_auth = base64Encode(auth_string);
    
    // Send AUTH PLAIN command
    if (!sendCommand("AUTH PLAIN " + encoded_auth)) {
        return false;
    }
    
    std::string response = readResponse();
    if (response[0] != '2') {
        logger.error("PLAIN authentication failed: " + response);
        return false;
    }
    
    logger.info("SMTP PLAIN authentication successful");
    return true;
}

SMTPResult SMTPClient::sendViaCurl(const Email& email, const DomainConfig* domain_config) {
    Logger& logger = Logger::getInstance();
    
    try {
        // Create temporary file for email content
        std::string temp_file = "/tmp/ssmtp_email_" + std::to_string(time(nullptr)) + ".txt";
        std::ofstream email_file(temp_file);
        
        if (!email_file.is_open()) {
            return SMTPResult::createError("Failed to create temporary email file");
        }
        
        // Write email headers and body
        email_file << "From: " << email.from << "\n";
        email_file << "To: ";
        for (size_t i = 0; i < email.to.size(); ++i) {
            if (i > 0) email_file << ", ";
            email_file << email.to[i];
        }
        email_file << "\n";
        email_file << "Subject: " << email.subject << "\n";
        email_file << "Date: " << getCurrentTimestamp() << "\n";
        email_file << "MIME-Version: 1.0\n";
        
        if (!email.html_body.empty()) {
            email_file << "Content-Type: multipart/alternative; boundary=\"boundary123\"\n";
            email_file << "\n";
            email_file << "--boundary123\n";
            email_file << "Content-Type: text/plain; charset=UTF-8\n";
            email_file << "\n";
            email_file << email.body << "\n";
            email_file << "--boundary123\n";
            email_file << "Content-Type: text/html; charset=UTF-8\n";
            email_file << "\n";
            email_file << email.html_body << "\n";
            email_file << "--boundary123--\n";
        } else {
            email_file << "Content-Type: text/plain; charset=UTF-8\n";
            email_file << "\n";
            email_file << email.body << "\n";
        }
        
        email_file.close();
        
        // Build curl command for SMTP
        std::ostringstream cmd;
        cmd << "curl -s --url smtp://" << domain_config->smtp_server << ":" << domain_config->smtp_port;
        
        // Add authentication if required
        if (domain_config->auth_method != "NONE" && !domain_config->username.empty()) {
            cmd << " --user " << domain_config->username << ":" << domain_config->password;
        }
        
        // Add SSL/TLS options
        if (domain_config->use_ssl) {
            cmd << " --ssl-reqd";
        } else if (domain_config->use_starttls) {
            cmd << " --ssl-reqd";
        }
        
        // Add mail options
        cmd << " --mail-from " << email.from;
        
        // Add recipients
        for (const auto& recipient : email.to) {
            cmd << " --mail-rcpt " << recipient;
        }
        
        // Add email content file
        cmd << " --upload-file " << temp_file;
        
        logger.info("Executing curl command: " + cmd.str());
        
        // Execute curl command
        int result = system(cmd.str().c_str());
        
        // Clean up temporary file
        unlink(temp_file.c_str());
        
        if (result == 0) {
            logger.info("Email sent successfully via curl SMTP");
            return SMTPResult::createSuccess("Email sent successfully via SMTP");
        } else {
            return SMTPResult::createError("curl SMTP command failed with exit code: " + std::to_string(result));
        }
        
    } catch (const std::exception& e) {
        return SMTPResult::createError("Curl SMTP error: " + std::string(e.what()));
    }
}

SMTPResult SMTPClient::sendEmailData(const Email& email) {
    Logger& logger = Logger::getInstance();
    
    // Send MAIL FROM command
    std::string mail_from = "MAIL FROM:<" + email.from + ">";
    if (!sendCommand(mail_from)) {
        return SMTPResult::createError("Failed to send MAIL FROM command");
    }
    
    std::string response = readResponse();
    if (response[0] != '2') {
        return SMTPResult::createError("MAIL FROM rejected: " + response);
    }
    
    // Send RCPT TO commands for each recipient
    for (const auto& recipient : email.to) {
        std::string rcpt_to = "RCPT TO:<" + recipient + ">";
        if (!sendCommand(rcpt_to)) {
            return SMTPResult::createError("Failed to send RCPT TO command");
        }
        
        response = readResponse();
        if (response[0] != '2') {
            return SMTPResult::createError("RCPT TO rejected: " + response);
        }
    }
    
    // Send DATA command
    if (!sendCommand("DATA")) {
        return SMTPResult::createError("Failed to send DATA command");
    }
    
    response = readResponse();
    if (response[0] != '3') {
        return SMTPResult::createError("DATA command rejected: " + response);
    }
    
    // Send email headers and body
    std::string email_data = buildEmailData(email);
    if (!sendCommand(email_data)) {
        return SMTPResult::createError("Failed to send email data");
    }
    
    // Send end of data marker
    if (!sendCommand(".")) {
        return SMTPResult::createError("Failed to send end of data marker");
    }
    
    response = readResponse();
    if (response[0] != '2') {
        return SMTPResult::createError("Email data rejected: " + response);
    }
    
    // Send QUIT command
    sendCommand("QUIT");
    
    logger.info("Email sent successfully");
    return SMTPResult::createSuccess("Email sent successfully");
}

std::string SMTPClient::buildEmailData(const Email& email) {
    std::ostringstream email_data;
    
    // Headers
    email_data << "From: " << email.from << "\r\n";
    email_data << "To: ";
    for (size_t i = 0; i < email.to.size(); ++i) {
        if (i > 0) email_data << ", ";
        email_data << email.to[i];
    }
    email_data << "\r\n";
    email_data << "Subject: " << email.subject << "\r\n";
    email_data << "Date: " << getCurrentTimestamp() << "\r\n";
    email_data << "MIME-Version: 1.0\r\n";
    
    if (!email.html_body.empty()) {
        email_data << "Content-Type: multipart/alternative; boundary=\"boundary123\"\r\n";
        email_data << "\r\n";
        email_data << "--boundary123\r\n";
        email_data << "Content-Type: text/plain; charset=UTF-8\r\n";
        email_data << "\r\n";
        email_data << email.body << "\r\n";
        email_data << "--boundary123\r\n";
        email_data << "Content-Type: text/html; charset=UTF-8\r\n";
        email_data << "\r\n";
        email_data << email.html_body << "\r\n";
        email_data << "--boundary123--\r\n";
    } else {
        email_data << "Content-Type: text/plain; charset=UTF-8\r\n";
        email_data << "\r\n";
        email_data << email.body << "\r\n";
    }
    
    return email_data.str();
}

std::string SMTPClient::getCurrentTimestamp() {
    time_t now = time(0);
    struct tm* timeinfo = gmtime(&now);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
    return std::string(buffer);
}

std::string SMTPClient::base64Encode(const std::string& input) {
    const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string result;
    int val = 0, valb = -6;
    
    for (unsigned char c : input) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            result.push_back(chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    
    if (valb > -6) {
        result.push_back(chars[((val << 8) >> (valb + 8)) & 0x3F]);
    }
    
    while (result.size() % 4) {
        result.push_back('=');
    }
    
    return result;
}

SMTPAuthMethod SMTPClient::stringToAuthMethod(const std::string& method) {
    if (method == "LOGIN") return SMTPAuthMethod::LOGIN;
    if (method == "PLAIN") return SMTPAuthMethod::PLAIN;
    if (method == "CRAM_MD5") return SMTPAuthMethod::CRAM_MD5;
    if (method == "OAUTH2") return SMTPAuthMethod::OAUTH2;
    if (method == "XOAUTH2") return SMTPAuthMethod::XOAUTH2;
    return SMTPAuthMethod::NONE;
}

} // namespace ssmtp_mailer
