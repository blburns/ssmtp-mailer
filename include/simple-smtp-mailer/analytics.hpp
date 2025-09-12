#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <chrono>
#include <atomic>
#include <mutex>

namespace ssmtp_mailer {

/**
 * @brief Analytics event type
 */
enum class AnalyticsEventType {
    EMAIL_SENT,         // Email sent successfully
    EMAIL_DELIVERED,    // Email delivered to recipient
    EMAIL_OPENED,       // Email opened by recipient
    EMAIL_CLICKED,      // Link clicked in email
    EMAIL_BOUNCED,      // Email bounced (hard/soft)
    EMAIL_DROPPED,      // Email dropped by provider
    EMAIL_SPAM,         // Marked as spam
    EMAIL_UNSUBSCRIBED, // Recipient unsubscribed
    EMAIL_DEFERRED,     // Delivery deferred
    EMAIL_PROCESSED,    // Email processed by provider
    RATE_LIMITED,       // Rate limited by provider
    AUTH_FAILED,        // Authentication failed
    CONNECTION_ERROR,   // Connection error
    TIMEOUT_ERROR,      // Timeout error
    UNKNOWN_ERROR       // Unknown error
};

/**
 * @brief Analytics event
 */
struct AnalyticsEvent {
    AnalyticsEventType type;
    std::string message_id;
    std::string provider;
    std::string sender;
    std::string recipient;
    std::string subject;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> metadata;
    int retry_count;
    std::string error_message;
    
    AnalyticsEvent() : type(AnalyticsEventType::UNKNOWN), retry_count(0) {}
};

/**
 * @brief Analytics metrics
 */
struct AnalyticsMetrics {
    // Counts
    std::atomic<int> total_emails_sent;
    std::atomic<int> total_emails_delivered;
    std::atomic<int> total_emails_opened;
    std::atomic<int> total_emails_clicked;
    std::atomic<int> total_emails_bounced;
    std::atomic<int> total_emails_dropped;
    std::atomic<int> total_emails_spam;
    std::atomic<int> total_emails_unsubscribed;
    std::atomic<int> total_emails_deferred;
    std::atomic<int> total_emails_processed;
    
    // Errors
    std::atomic<int> total_rate_limited;
    std::atomic<int> total_auth_failures;
    std::atomic<int> total_connection_errors;
    std::atomic<int> total_timeout_errors;
    std::atomic<int> total_unknown_errors;
    
    // Performance
    std::atomic<int> total_retries;
    std::atomic<int> total_fallbacks;
    
    // Provider-specific
    std::map<std::string, int> provider_success_counts;
    std::map<std::string, int> provider_failure_counts;
    
    AnalyticsMetrics() {
        // Initialize all atomic counters to 0
        total_emails_sent = 0;
        total_emails_delivered = 0;
        total_emails_opened = 0;
        total_emails_clicked = 0;
        total_emails_bounced = 0;
        total_emails_dropped = 0;
        total_emails_spam = 0;
        total_emails_unsubscribed = 0;
        total_emails_deferred = 0;
        total_emails_processed = 0;
        total_rate_limited = 0;
        total_auth_failures = 0;
        total_connection_errors = 0;
        total_timeout_errors = 0;
        total_unknown_errors = 0;
        total_retries = 0;
        total_fallbacks = 0;
    }
};

/**
 * @brief Time-based analytics data
 */
struct TimeSeriesData {
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, int> metrics;
    
    TimeSeriesData() = default;
    TimeSeriesData(const std::chrono::system_clock::time_point& ts) : timestamp(ts) {}
};

/**
 * @brief Analytics configuration
 */
struct AnalyticsConfig {
    bool enable_tracking;
    bool enable_persistence;
    bool enable_real_time;
    std::string storage_path;
    int retention_days;
    int flush_interval_seconds;
    int max_events_in_memory;
    
    AnalyticsConfig() : enable_tracking(true), enable_persistence(true), 
                       enable_real_time(true), retention_days(90),
                       flush_interval_seconds(60), max_events_in_memory(10000) {}
};

/**
 * @brief Analytics event callback
 */
using AnalyticsEventCallback = std::function<void(const AnalyticsEvent&)>;

/**
 * @brief Analytics manager
 */
class AnalyticsManager {
public:
    /**
     * @brief Constructor
     * @param config Analytics configuration
     */
    explicit AnalyticsManager(const AnalyticsConfig& config);
    
    /**
     * @brief Destructor
     */
    ~AnalyticsManager();
    
    /**
     * @brief Track event
     * @param event Event to track
     */
    void trackEvent(const AnalyticsEvent& event);
    
    /**
     * @brief Track email sent
     * @param message_id Message ID
     * @param provider Provider name
     * @param sender Sender email
     * @param recipient Recipient email
     * @param subject Email subject
     * @param metadata Additional metadata
     */
    void trackEmailSent(const std::string& message_id, 
                       const std::string& provider,
                       const std::string& sender,
                       const std::string& recipient,
                       const std::string& subject,
                       const std::map<std::string, std::string>& metadata = {});
    
    /**
     * @brief Track email delivered
     * @param message_id Message ID
     * @param provider Provider name
     * @param metadata Additional metadata
     */
    void trackEmailDelivered(const std::string& message_id,
                           const std::string& provider,
                           const std::map<std::string, std::string>& metadata = {});
    
    /**
     * @brief Track email opened
     * @param message_id Message ID
     * @param provider Provider name
     * @param metadata Additional metadata
     */
    void trackEmailOpened(const std::string& message_id,
                         const std::string& provider,
                         const std::map<std::string, std::string>& metadata = {});
    
    /**
     * @brief Track email clicked
     * @param message_id Message ID
     * @param provider Provider name
     * @param link_url Clicked link URL
     * @param metadata Additional metadata
     */
    void trackEmailClicked(const std::string& message_id,
                          const std::string& provider,
                          const std::string& link_url,
                          const std::map<std::string, std::string>& metadata = {});
    
    /**
     * @brief Track email bounced
     * @param message_id Message ID
     * @param provider Provider name
     * @param reason Bounce reason
     * @param is_hard_bounce Whether it's a hard bounce
     * @param metadata Additional metadata
     */
    void trackEmailBounced(const std::string& message_id,
                          const std::string& provider,
                          const std::string& reason,
                          bool is_hard_bounce,
                          const std::map<std::string, std::string>& metadata = {});
    
    /**
     * @brief Track error
     * @param event_type Error event type
     * @param provider Provider name
     * @param error_message Error message
     * @param metadata Additional metadata
     */
    void trackError(AnalyticsEventType event_type,
                   const std::string& provider,
                   const std::string& error_message,
                   const std::map<std::string, std::string>& metadata = {});
    
    /**
     * @brief Get current metrics
     * @return Current analytics metrics
     */
    AnalyticsMetrics getCurrentMetrics() const;
    
    /**
     * @brief Get metrics for time period
     * @param start_time Start time
     * @param end_time End time
     * @return Metrics for the period
     */
    AnalyticsMetrics getMetricsForPeriod(const std::chrono::system_clock::time_point& start_time,
                                       const std::chrono::system_clock::time_point& end_time) const;
    
    /**
     * @brief Get time series data
     * @param start_time Start time
     * @param end_time End time
     * @param interval_minutes Time interval in minutes
     * @return Time series data
     */
    std::vector<TimeSeriesData> getTimeSeriesData(const std::chrono::system_clock::time_point& start_time,
                                                 const std::chrono::system_clock::time_point& end_time,
                                                 int interval_minutes = 60) const;
    
    /**
     * @brief Get provider performance
     * @return Map of provider performance metrics
     */
    std::map<std::string, std::map<std::string, double>> getProviderPerformance() const;
    
    /**
     * @brief Get delivery rates
     * @return Map of delivery rate metrics
     */
    std::map<std::string, double> getDeliveryRates() const;
    
    /**
     * @brief Get engagement rates
     * @return Map of engagement rate metrics
     */
    std::map<std::string, double> getEngagementRates() const;
    
    /**
     * @brief Register event callback
     * @param event_type Event type to listen for
     * @param callback Callback function
     */
    void registerCallback(AnalyticsEventType event_type, AnalyticsEventCallback callback);
    
    /**
     * @brief Generate report
     * @param report_type Report type
     * @param format Output format (json, csv, html)
     * @param start_time Start time for report
     * @param end_time End time for report
     * @return Generated report content
     */
    std::string generateReport(const std::string& report_type,
                             const std::string& format,
                             const std::chrono::system_clock::time_point& start_time,
                             const std::chrono::system_clock::time_point& end_time) const;
    
    /**
     * @brief Export data
     * @param format Export format
     * @param file_path Output file path
     * @param start_time Start time for export
     * @param end_time End time for export
     * @return true if export successful
     */
    bool exportData(const std::string& format,
                   const std::string& file_path,
                   const std::chrono::system_clock::time_point& start_time,
                   const std::chrono::system_clock::time_point& end_time) const;
    
    /**
     * @brief Clear old data
     * @param days_to_keep Days of data to keep
     * @return Number of events cleared
     */
    int clearOldData(int days_to_keep);
    
    /**
     * @brief Reset analytics
     */
    void reset();
    
    /**
     * @brief Update configuration
     * @param config New configuration
     */
    void updateConfig(const AnalyticsConfig& config);

private:
    AnalyticsConfig config_;
    AnalyticsMetrics metrics_;
    std::vector<AnalyticsEvent> events_;
    std::map<AnalyticsEventType, std::vector<AnalyticsEventCallback>> callbacks_;
    mutable std::mutex mutex_;
    
    // Background processing
    std::thread background_thread_;
    std::atomic<bool> running_;
    std::condition_variable cv_;
    
    // Helper methods
    void backgroundProcessor();
    void persistEvents();
    void loadEvents();
    void cleanupOldEvents();
    void notifyCallbacks(const AnalyticsEvent& event);
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const;
    std::string generateJSONReport(const std::chrono::system_clock::time_point& start_time,
                                 const std::chrono::system_clock::time_point& end_time) const;
    std::string generateCSVReport(const std::chrono::system_clock::time_point& start_time,
                                const std::chrono::system_clock::time_point& end_time) const;
    std::string generateHTMLReport(const std::chrono::system_clock::time_point& start_time,
                                 const std::chrono::system_clock::time_point& end_time) const;
};

/**
 * @brief Analytics event formatter
 */
class AnalyticsEventFormatter {
public:
    /**
     * @brief Format event as JSON
     * @param event Event to format
     * @return JSON string
     */
    static std::string toJSON(const AnalyticsEvent& event);
    
    /**
     * @brief Format event as CSV
     * @param event Event to format
     * @return CSV string
     */
    static std::string toCSV(const AnalyticsEvent& event);
    
    /**
     * @brief Format event as human-readable text
     * @param event Event to format
     * @return Human-readable string
     */
    static std::string toHumanReadable(const AnalyticsEvent& event);
    
    /**
     * @brief Get event type name
     * @param event_type Event type
     * @return Event type name
     */
    static std::string getEventTypeName(AnalyticsEventType event_type);
};

} // namespace ssmtp_mailer
