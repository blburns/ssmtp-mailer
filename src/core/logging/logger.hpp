#pragma once

#include <string>
#include <memory>
#include <fstream>
#include <mutex>
#include <chrono>
#include <sstream>

namespace ssmtp_mailer {

/**
 * @brief Log levels
 */
enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    CRITICAL = 4
};

/**
 * @brief Logger class for structured logging
 */
class Logger {
public:
    /**
     * @brief Initialize the logger
     * @param log_file Log file path (optional, uses stderr if not specified)
     * @param level Minimum log level to output
     */
    static void initialize(const std::string& log_file = "", LogLevel level = LogLevel::INFO);
    
    /**
     * @brief Get logger instance
     * @return Reference to logger instance
     */
    static Logger& getInstance();
    
    /**
     * @brief Destructor
     */
    ~Logger();
    
    /**
     * @brief Set log level
     * @param level New log level
     */
    void setLogLevel(LogLevel level);
    
    /**
     * @brief Get current log level
     * @return Current log level
     */
    LogLevel getLogLevel() const;
    
    /**
     * @brief Set log file
     * @param log_file Path to log file
     * @return true if successful, false otherwise
     */
    bool setLogFile(const std::string& log_file);
    
    /**
     * @brief Log debug message
     * @param message Message to log
     */
    void debug(const std::string& message);
    
    /**
     * @brief Log info message
     * @param message Message to log
     */
    void info(const std::string& message);
    
    /**
     * @brief Log warning message
     * @param message Message to log
     */
    void warning(const std::string& message);
    
    /**
     * @brief Log error message
     * @param message Message to log
     */
    void error(const std::string& message);
    
    /**
     * @brief Log critical message
     * @param message Message to log
     */
    void critical(const std::string& message);
    
    /**
     * @brief Log message with custom level
     * @param level Log level
     * @param message Message to log
     */
    void log(LogLevel level, const std::string& message);
    
    /**
     * @brief Enable/disable console output
     * @param enable Whether to enable console output
     */
    void enableConsole(bool enable);
    
    /**
     * @brief Enable/disable file output
     * @param enable Whether to enable file output
     */
    void enableFile(bool enable);
    
    /**
     * @brief Enable/disable timestamp in log messages
     * @param enable Whether to enable timestamps
     */
    void enableTimestamp(bool enable);
    
    /**
     * @brief Enable/disable log level in log messages
     * @param enable Whether to enable log levels
     */
    void enableLogLevel(bool enable);
    
    /**
     * @brief Enable/disable thread ID in log messages
     * @param enable Whether to enable thread IDs
     */
    void enableThreadId(bool enable);
    
    /**
     * @brief Set log format
     * @param format Log format string (supports placeholders: {timestamp}, {level}, {thread}, {message})
     */
    void setLogFormat(const std::string& format);
    
    /**
     * @brief Flush log buffer
     */
    void flush();
    
    /**
     * @brief Rotate log file
     * @param max_size Maximum size in bytes before rotation
     * @param max_files Maximum number of backup files
     * @return true if successful, false otherwise
     */
    bool rotateLog(size_t max_size = 10 * 1024 * 1024, size_t max_files = 5);

private:
    /**
     * @brief Private constructor (singleton)
     */
    Logger();
    
    /**
     * @brief Copy constructor (deleted)
     */
    Logger(const Logger&) = delete;
    
    /**
     * @brief Assignment operator (deleted)
     */
    Logger& operator=(const Logger&) = delete;
    
    /**
     * @brief Move constructor (deleted)
     */
    Logger(Logger&&) = delete;
    
    /**
     * @brief Move assignment operator (deleted)
     */
    Logger& operator=(Logger&&) = delete;
    
    /**
     * @brief Format log message
     * @param level Log level
     * @param message Message to format
     * @return Formatted message
     */
    std::string formatMessage(LogLevel level, const std::string& message) const;
    
    /**
     * @brief Get current timestamp string
     * @return Timestamp string
     */
    std::string getCurrentTimestamp() const;
    
    /**
     * @brief Get log level string
     * @param level Log level
     * @return Log level string
     */
    std::string getLogLevelString(LogLevel level) const;
    
    /**
     * @brief Get current thread ID string
     * @return Thread ID string
     */
    std::string getCurrentThreadId() const;
    
    /**
     * @brief Write message to output
     * @param message Message to write
     */
    void writeMessage(const std::string& message);
    
    /**
     * @brief Check if log level should be output
     * @param level Log level to check
     * @return true if should output, false otherwise
     */
    bool shouldOutput(LogLevel level) const;

private:
    static std::unique_ptr<Logger> instance_;
    static std::mutex instance_mutex_;
    
    LogLevel current_level_;
    std::string log_file_;
    std::ofstream log_stream_;
    std::mutex log_mutex_;
    
    bool console_enabled_;
    bool file_enabled_;
    bool timestamp_enabled_;
    bool log_level_enabled_;
    bool thread_id_enabled_;
    
    std::string log_format_;
    std::string default_format_;
    
    size_t max_log_size_;
    size_t max_log_files_;
};

/**
 * @brief Stream-based logging macros
 */
#define LOG_DEBUG(logger) LoggerStream(logger, LogLevel::DEBUG)
#define LOG_INFO(logger) LoggerStream(logger, LogLevel::INFO)
#define LOG_WARNING(logger) LoggerStream(logger, LogLevel::WARNING)
#define LOG_ERROR(logger) LoggerStream(logger, LogLevel::ERROR)
#define LOG_CRITICAL(logger) LoggerStream(logger, LogLevel::CRITICAL)

/**
 * @brief Logger stream class for stream-based logging
 */
class LoggerStream {
public:
    /**
     * @brief Constructor
     * @param logger Logger instance
     * @param level Log level
     */
    LoggerStream(Logger& logger, LogLevel level);
    
    /**
     * @brief Destructor
     */
    ~LoggerStream();
    
    /**
     * @brief Stream operator for various types
     */
    template<typename T>
    LoggerStream& operator<<(const T& value) {
        stream_ << value;
        return *this;
    }
    
    /**
     * @brief Stream operator for manipulators
     */
    LoggerStream& operator<<(std::ostream& (*manipulator)(std::ostream&));

private:
    Logger& logger_;
    LogLevel level_;
    std::ostringstream stream_;
};

} // namespace ssmtp_mailer
