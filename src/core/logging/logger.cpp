#include "core/logging/logger.hpp"
#include <iostream>
#include <mutex>

namespace ssmtp_mailer {

static Logger* g_instance = nullptr;
static std::mutex g_instance_mutex;
static LogLevel g_log_level = LogLevel::INFO;

void Logger::initialize(const std::string& log_file, LogLevel level) {
    (void)log_file; // Suppress unused parameter warning
    std::lock_guard<std::mutex> lock(g_instance_mutex);
    if (!g_instance) {
        g_instance = new Logger();
    }
    g_log_level = level;
}

Logger::Logger() {
    // Default constructor
}

Logger& Logger::getInstance() {
    std::lock_guard<std::mutex> lock(g_instance_mutex);
    if (!g_instance) {
        g_instance = new Logger();
    }
    return *g_instance;
}

Logger::~Logger() {
    // Cleanup if needed
}

void Logger::setLogLevel(LogLevel level) {
    g_log_level = level;
}

LogLevel Logger::getLogLevel() const {
    return g_log_level;
}

bool Logger::setLogFile(const std::string& log_file) {
    (void)log_file; // Suppress unused parameter warning
    // Stub implementation
    return true;
}

void Logger::debug(const std::string& message) {
    if (g_log_level <= LogLevel::DEBUG) {
        std::cout << "[DEBUG] " << message << std::endl;
    }
}

void Logger::info(const std::string& message) {
    if (g_log_level <= LogLevel::INFO) {
        std::cout << "[INFO] " << message << std::endl;
    }
}

void Logger::warning(const std::string& message) {
    if (g_log_level <= LogLevel::WARNING) {
        std::cout << "[WARNING] " << message << std::endl;
    }
}

void Logger::error(const std::string& message) {
    if (g_log_level <= LogLevel::ERROR) {
        std::cout << "[ERROR] " << message << std::endl;
    }
}

void Logger::critical(const std::string& message) {
    if (g_log_level <= LogLevel::CRITICAL) {
        std::cout << "[CRITICAL] " << message << std::endl;
    }
}

void Logger::log(LogLevel level, const std::string& message) {
    switch (level) {
        case LogLevel::DEBUG: debug(message); break;
        case LogLevel::INFO: info(message); break;
        case LogLevel::WARNING: warning(message); break;
        case LogLevel::ERROR: error(message); break;
        case LogLevel::CRITICAL: critical(message); break;
    }
}

} // namespace ssmtp_mailer
