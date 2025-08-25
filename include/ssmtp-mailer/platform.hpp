#pragma once

#include <string>
#include <memory>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    #include <iphlpapi.h>
    #include <crypt32.h>
    #include <advapi32.h>
    #include <userenv.h>
    #include <shell32.h>
    
    // Windows type definitions
    using socket_t = SOCKET;
    using ssize_t = SSIZE_T;
    
    // Windows error handling
    #define SOCKET_ERROR_CODE WSAGetLastError()
    #define SOCKET_ERROR_MSG std::string("Windows Socket Error: ") + std::to_string(WSAGetLastError())
    
    // Windows socket initialization
    inline bool initializeWinsock() {
        WSADATA wsaData;
        return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
    }
    
    inline void cleanupWinsock() {
        WSACleanup();
    }
    
    // Windows socket close
    inline int closeSocket(socket_t sock) {
        return closesocket(sock);
    }
    
    // Windows socket operations
    inline int sendData(socket_t sock, const char* data, size_t length) {
        return send(sock, data, static_cast<int>(length), 0);
    }
    
    inline int receiveData(socket_t sock, char* buffer, size_t max_length) {
        return recv(sock, buffer, static_cast<int>(max_length), 0);
    }
    
    // Windows file operations
    inline std::string getConfigDir() {
        char appDataPath[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA, NULL, 0, appDataPath))) {
            return std::string(appDataPath) + "\\ssmtp-mailer";
        }
        return "C:\\Program Files\\ssmtp-mailer\\config";
    }
    
    inline std::string getLogDir() {
        char appDataPath[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA, NULL, 0, appDataPath))) {
            return std::string(appDataPath) + "\\ssmtp-mailer\\logs";
        }
        return "C:\\Program Files\\ssmtp-mailer\\logs";
    }
    
#else
    // POSIX systems (Linux, macOS)
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <cstring>
    
    // POSIX type definitions
    using socket_t = int;
    
    // POSIX error handling
    #define SOCKET_ERROR_CODE errno
    #define SOCKET_ERROR_MSG std::string("POSIX Error: ") + std::strerror(errno)
    
    // POSIX socket initialization (no-op)
    inline bool initializeWinsock() { return true; }
    inline void cleanupWinsock() {}
    
    // POSIX socket close
    inline int closeSocket(socket_t sock) {
        return close(sock);
    }
    
    // POSIX socket operations
    inline int sendData(socket_t sock, const char* data, size_t length) {
        return send(sock, data, length, 0);
    }
    
    inline int receiveData(socket_t sock, char* buffer, size_t max_length) {
        return recv(sock, buffer, max_length, 0);
    }
    
    // POSIX file operations
    inline std::string getConfigDir() {
        #ifdef __APPLE__
            return "/usr/local/etc/ssmtp-mailer";
        #else
            return "/etc/ssmtp-mailer";
        #endif
    }
    
    inline std::string getLogDir() {
        return "/var/log";
    }
#endif

namespace ssmtp_mailer {

/**
 * @brief Platform abstraction class
 */
class Platform {
public:
    /**
     * @brief Initialize platform-specific resources
     * @return true if successful, false otherwise
     */
    static bool initialize();
    
    /**
     * @brief Cleanup platform-specific resources
     */
    static void cleanup();
    
    /**
     * @brief Get platform name
     * @return Platform name string
     */
    static std::string getPlatformName();
    
    /**
     * @brief Get configuration directory
     * @return Configuration directory path
     */
    static std::string getConfigDirectory();
    
    /**
     * @brief Get log directory
     * @return Log directory path
     */
    static std::string getLogDirectory();
    
    /**
     * @brief Create directory if it doesn't exist
     * @param path Directory path to create
     * @return true if successful, false otherwise
     */
    static bool createDirectory(const std::string& path);
    
    /**
     * @brief Check if file exists
     * @param path File path to check
     * @return true if exists, false otherwise
     */
    static bool fileExists(const std::string& path);
    
    /**
     * @brief Get file size
     * @param path File path
     * @return File size in bytes, -1 if error
     */
    static ssize_t getFileSize(const std::string& path);
    
    /**
     * @brief Get current working directory
     * @return Current working directory path
     */
    static std::string getCurrentWorkingDirectory();
    
    /**
     * @brief Get environment variable
     * @param name Environment variable name
     * @return Environment variable value, empty if not found
     */
    static std::string getEnvironmentVariable(const std::string& name);
    
    /**
     * @brief Set environment variable
     * @param name Environment variable name
     * @param value Environment variable value
     * @return true if successful, false otherwise
     */
    static bool setEnvironmentVariable(const std::string& name, const std::string& value);
};

} // namespace ssmtp_mailer
