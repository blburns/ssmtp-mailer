#include "ssmtp-mailer/platform.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

#ifdef _WIN32
    #include <direct.h>
    #include <io.h>
    #include <shlobj.h>
    #include <sys/stat.h>
#else
    #include <sys/stat.h>
    #include <unistd.h>
    #include <pwd.h>
    #include <cstdlib>
#endif

namespace ssmtp_mailer {

bool Platform::initialize() {
    #ifdef _WIN32
        return initializeWinsock();
    #else
        return true;
    #endif
}

void Platform::cleanup() {
    #ifdef _WIN32
        cleanupWinsock();
    #endif
}

std::string Platform::getPlatformName() {
    #ifdef _WIN32
        return "Windows";
    #elif defined(__APPLE__)
        return "macOS";
    #elif defined(__linux__)
        return "Linux";
    #else
        return "Unknown";
    #endif
}

std::string Platform::getConfigDirectory() {
    return getConfigDir();
}

std::string Platform::getLogDirectory() {
    return getLogDir();
}

bool Platform::createDirectory(const std::string& path) {
    #ifdef _WIN32
        return _mkdir(path.c_str()) == 0;
    #else
        return mkdir(path.c_str(), 0755) == 0;
    #endif
}

bool Platform::fileExists(const std::string& path) {
    #ifdef _WIN32
        return _access(path.c_str(), 0) == 0;
    #else
        return access(path.c_str(), F_OK) == 0;
    #endif
}

ssize_t Platform::getFileSize(const std::string& path) {
    #ifdef _WIN32
        struct _stat fileStat;
        if (_stat(path.c_str(), &fileStat) == 0) {
            return fileStat.st_size;
        }
    #else
        struct stat fileStat;
        if (stat(path.c_str(), &fileStat) == 0) {
            return fileStat.st_size;
        }
    #endif
    return -1;
}

std::string Platform::getCurrentWorkingDirectory() {
    #ifdef _WIN32
        char buffer[MAX_PATH];
        if (_getcwd(buffer, MAX_PATH) != nullptr) {
            return std::string(buffer);
        }
    #else
        char buffer[PATH_MAX];
        if (getcwd(buffer, PATH_MAX) != nullptr) {
            return std::string(buffer);
        }
    #endif
    return "";
}

std::string Platform::getEnvironmentVariable(const std::string& name) {
    #ifdef _WIN32
        char buffer[32767];
        DWORD result = GetEnvironmentVariableA(name.c_str(), buffer, sizeof(buffer));
        if (result > 0 && result < sizeof(buffer)) {
            return std::string(buffer);
        }
    #else
        const char* value = std::getenv(name.c_str());
        if (value != nullptr) {
            return std::string(value);
        }
    #endif
    return "";
}

bool Platform::setEnvironmentVariable(const std::string& name, const std::string& value) {
    #ifdef _WIN32
        return SetEnvironmentVariableA(name.c_str(), value.c_str()) != 0;
    #else
        return setenv(name.c_str(), value.c_str(), 1) == 0;
    #endif
}

} // namespace ssmtp_mailer
