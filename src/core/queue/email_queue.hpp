#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <chrono>
#include <functional>
#include "ssmtp-mailer/queue_types.hpp"
#include "ssmtp-mailer/mailer.hpp"

namespace ssmtp_mailer {

// Queue types are now defined in queue_types.hpp

class EmailQueue {
public:
    EmailQueue();
    ~EmailQueue();

    // Queue management
    void enqueue(const Email* email, EmailPriority priority = EmailPriority::NORMAL);
    bool dequeue(QueueItem& email);
    size_t size() const;
    bool empty() const;
    
    // Queue processing
    void start();
    void stop();
    bool isRunning() const;
    
    // Configuration
    void setMaxRetries(int max_retries);
    void setRetryDelay(std::chrono::seconds delay);
    void setBatchSize(size_t batch_size);
    void setMaxQueueSize(size_t max_size);
    
    // Statistics
    size_t getTotalProcessed() const;
    size_t getTotalFailed() const;
    size_t getTotalRetries() const;
    
    // Callbacks
    using SendCallback = std::function<SMTPResult(const Email*)>;
    void setSendCallback(SendCallback callback);
    
    // Queue inspection
    std::vector<QueueItem> getPendingEmails() const;
    std::vector<QueueItem> getFailedEmails() const;

private:
    // Queue storage
    mutable std::mutex queue_mutex_;
    std::priority_queue<QueueItem, std::vector<QueueItem>, 
                       std::function<bool(const QueueItem&, const QueueItem&)>> email_queue_;
    
    // Processing state
    std::atomic<bool> running_;
    std::thread worker_thread_;
    std::condition_variable queue_cv_;
    
    // Configuration
    int max_retries_;
    std::chrono::seconds retry_delay_;
    size_t batch_size_;
    size_t max_queue_size_;
    
    // Statistics
    std::atomic<size_t> total_processed_;
    std::atomic<size_t> total_failed_;
    std::atomic<size_t> total_retries_;
    
    // Callbacks
    SendCallback send_callback_;
    
    // Worker thread function
    void workerLoop();
    
    // Helper methods
    void processEmail(QueueItem& queued_email);
    bool shouldRetry(const QueueItem& queued_email) const;
    void updateRetryInfo(QueueItem& queued_email);
    
    // Priority comparison function
    static bool comparePriority(const QueueItem& a, const QueueItem& b);
};

} // namespace ssmtp_mailer
