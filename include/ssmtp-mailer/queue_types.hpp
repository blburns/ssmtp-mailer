#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <memory>

namespace ssmtp_mailer {

/**
 * @brief Email priority levels
 */
enum class EmailPriority {
    LOW = 0,
    NORMAL = 1,
    HIGH = 2,
    URGENT = 3
};

/**
 * @brief Email status in the queue
 */
enum class EmailStatus {
    PENDING = 0,
    PROCESSING = 1,
    SENT = 2,
    FAILED = 3,
    RETRY = 4,
    CANCELLED = 5
};

/**
 * @brief Queue item structure
 */
struct QueueItem {
    std::string id;
    std::string domain;
    std::string user;
    std::string from_address;
    std::vector<std::string> to_addresses;
    std::string subject;
    std::string body;
    std::string html_body;
    std::vector<std::string> attachments;
    EmailPriority priority;
    EmailStatus status;
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point scheduled_for;
    int retry_count;
    int max_retries;
    std::string error_message;
    
    QueueItem() = default;
    
    QueueItem(const std::string& from, 
              const std::vector<std::string>& to,
              const std::string& subj,
              const std::string& body_text)
        : from_address(from), to_addresses(to), subject(subj), body(body_text),
          priority(EmailPriority::NORMAL), status(EmailStatus::PENDING),
          created_at(std::chrono::system_clock::now()),
          retry_count(0), max_retries(3) {}
};

/**
 * @brief Queue configuration
 */
struct QueueConfig {
    size_t max_queue_size;
    size_t max_workers;
    std::chrono::seconds retry_delay;
    std::chrono::seconds max_retry_delay;
    bool enable_priority_queuing;
    bool enable_scheduled_sending;
    
    QueueConfig()
        : max_queue_size(10000), max_workers(4),
          retry_delay(std::chrono::seconds(60)),
          max_retry_delay(std::chrono::seconds(3600)),
          enable_priority_queuing(true),
          enable_scheduled_sending(true) {}
};

/**
 * @brief Queue statistics
 */
struct QueueStats {
    size_t total_queued;
    size_t total_sent;
    size_t total_failed;
    size_t total_retried;
    size_t current_queue_size;
    size_t active_workers;
    std::chrono::system_clock::time_point last_activity;
    
    QueueStats()
        : total_queued(0), total_sent(0), total_failed(0),
          total_retried(0), current_queue_size(0), active_workers(0),
          last_activity(std::chrono::system_clock::now()) {}
};

} // namespace ssmtp_mailer
