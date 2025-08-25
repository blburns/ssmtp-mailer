#include "core/queue/email_queue.hpp"
#include "core/logging/logger.hpp"
#include "ssmtp-mailer/mailer.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>

namespace ssmtp_mailer {

EmailQueue::EmailQueue()
    : email_queue_(comparePriority), running_(false), max_retries_(3),
      retry_delay_(std::chrono::seconds(300)), batch_size_(10), max_queue_size_(1000),
      total_processed_(0), total_failed_(0), total_retries_(0) {
    
    Logger& logger = Logger::getInstance();
    logger.debug("EmailQueue initialized");
}

EmailQueue::~EmailQueue() {
    stop();
}

void EmailQueue::enqueue(const Email* email, EmailPriority priority) {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    
    if (email_queue_.size() >= max_queue_size_) {
        Logger& logger = Logger::getInstance();
        logger.warning("Queue is full, rejecting email from: " + email->from);
        return;
    }
    
    QueueItem queued_email(email, priority);
    email_queue_.push(queued_email);
    
    Logger& logger = Logger::getInstance();
    logger.debug("Email queued from: " + email->from + " with priority: " + 
                std::to_string(static_cast<int>(priority)) + 
                " (queue size: " + std::to_string(email_queue_.size()) + ")");
    
    queue_cv_.notify_one();
}

bool EmailQueue::dequeue(QueueItem& email) {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    
    if (email_queue_.empty()) {
        return false;
    }
    
    email = email_queue_.top();
    email_queue_.pop();
    
    return true;
}

size_t EmailQueue::size() const {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    return email_queue_.size();
}

bool EmailQueue::empty() const {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    return email_queue_.empty();
}

void EmailQueue::start() {
    if (running_) {
        return;
    }
    
    running_ = true;
    worker_thread_ = std::thread(&EmailQueue::workerLoop, this);
    
    Logger& logger = Logger::getInstance();
    logger.info("EmailQueue worker thread started");
}

void EmailQueue::stop() {
    if (!running_) {
        return;
    }
    
    running_ = false;
    queue_cv_.notify_all();
    
    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }
    
    Logger& logger = Logger::getInstance();
    logger.info("EmailQueue worker thread stopped");
}

bool EmailQueue::isRunning() const {
    return running_;
}

void EmailQueue::setMaxRetries(int max_retries) {
    max_retries_ = max_retries;
}

void EmailQueue::setRetryDelay(std::chrono::seconds delay) {
    retry_delay_ = delay;
}

void EmailQueue::setBatchSize(size_t batch_size) {
    batch_size_ = batch_size;
}

void EmailQueue::setMaxQueueSize(size_t max_size) {
    max_queue_size_ = max_size;
}

size_t EmailQueue::getTotalProcessed() const {
    return total_processed_;
}

size_t EmailQueue::getTotalFailed() const {
    return total_failed_;
}

size_t EmailQueue::getTotalRetries() const {
    return total_retries_;
}

void EmailQueue::setSendCallback(SendCallback callback) {
    send_callback_ = callback;
}

std::vector<QueuedEmail> EmailQueue::getPendingEmails() const {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    
    std::vector<QueuedEmail> pending_emails;
    std::priority_queue<QueuedEmail, std::vector<QueuedEmail>, 
                       std::function<bool(const QueuedEmail&, const QueuedEmail&)>> temp_queue = email_queue_;
    
    while (!temp_queue.empty()) {
        QueuedEmail email = temp_queue.top();
        if (email.status == QueueStatus::PENDING || email.status == QueueStatus::RETRY) {
            pending_emails.push_back(email);
        }
        temp_queue.pop();
    }
    
    return pending_emails;
}

std::vector<QueuedEmail> EmailQueue::getFailedEmails() const {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    
    std::vector<QueuedEmail> failed_emails;
    std::priority_queue<QueuedEmail, std::vector<QueuedEmail>, 
                       std::function<bool(const QueuedEmail&, const QueuedEmail&)>> temp_queue = email_queue_;
    
    while (!temp_queue.empty()) {
        QueuedEmail email = temp_queue.top();
        if (email.status == QueueStatus::FAILED) {
            failed_emails.push_back(email);
        }
        temp_queue.pop();
    }
    
    return failed_emails;
}

void EmailQueue::workerLoop() {
    Logger& logger = Logger::getInstance();
    logger.debug("EmailQueue worker loop started");
    
    while (running_) {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        
        // Wait for emails or stop signal
        queue_cv_.wait(lock, [this] { return !running_ || !email_queue_.empty(); });
        
        if (!running_) {
            break;
        }
        
        // Process emails in batches
        std::vector<QueuedEmail> batch;
        for (size_t i = 0; i < batch_size_ && !email_queue_.empty(); ++i) {
            QueuedEmail email = email_queue_.top();
            email_queue_.pop();
            
            // Check if email is ready for processing
            if (email.status == QueueStatus::RETRY) {
                auto now = std::chrono::system_clock::now();
                auto time_since_last_attempt = std::chrono::duration_cast<std::chrono::seconds>(
                    now - email.last_attempt);
                
                if (time_since_last_attempt < email.retry_delay) {
                    // Not ready for retry, put back in queue
                    email_queue_.push(email);
                    continue;
                }
            }
            
            batch.push_back(email);
        }
        
        lock.unlock();
        
        // Process batch
        for (auto& queued_email : batch) {
            if (!running_) {
                break;
            }
            
            processEmail(queued_email);
        }
        
        // Small delay to prevent busy waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    logger.debug("EmailQueue worker loop ended");
}

void EmailQueue::processEmail(QueuedEmail& queued_email) {
    Logger& logger = Logger::getInstance();
    
    if (!send_callback_) {
        logger.error("No send callback set, cannot process email");
        queued_email.status = QueueStatus::FAILED;
        queued_email.last_error = "No send callback configured";
        total_failed_++;
        return;
    }
    
    queued_email.status = QueueStatus::PROCESSING;
    queued_email.last_attempt = std::chrono::system_clock::now();
    
    logger.debug("Processing email from: " + queued_email.email->from + 
                " to: " + (queued_email.email->getAllRecipients().empty() ? "none" : queued_email.email->getAllRecipients()[0]));
    
    try {
        SMTPResult result = send_callback_(queued_email.email);
        
        if (result.success) {
            queued_email.status = QueueStatus::SENT;
            total_processed_++;
            logger.info("Email sent successfully from: " + queued_email.email->from);
        } else {
            if (shouldRetry(queued_email)) {
                queued_email.status = QueueStatus::RETRY;
                updateRetryInfo(queued_email);
                total_retries_++;
                
                // Re-queue for retry
                std::lock_guard<std::mutex> lock(queue_mutex_);
                email_queue_.push(queued_email);
                
                logger.warning("Email queued for retry from: " + queued_email.email->from + 
                              " (attempt " + std::to_string(queued_email.retry_count) + "/" + 
                              std::to_string(queued_email.max_retries) + ")");
            } else {
                queued_email.status = QueueStatus::FAILED;
                queued_email.last_error = result.error_message;
                total_failed_++;
                
                logger.error("Email failed permanently from: " + queued_email.email->from + 
                            ": " + result.error_message);
            }
        }
    } catch (const std::exception& e) {
        queued_email.status = QueueStatus::FAILED;
        queued_email.last_error = "Exception: " + std::string(e.what());
        total_failed_++;
        
        logger.error("Exception while processing email from: " + queued_email.email->from + 
                    ": " + e.what());
    }
}

bool EmailQueue::shouldRetry(const QueuedEmail& queued_email) const {
    return queued_email.retry_count < queued_email.max_retries;
}

void EmailQueue::updateRetryInfo(QueuedEmail& queued_email) {
    queued_email.retry_count++;
    
    // Exponential backoff: double the delay for each retry
    auto backoff_multiplier = std::pow(2, queued_email.retry_count - 1);
    queued_email.retry_delay = std::chrono::seconds(
        static_cast<long long>(queued_email.retry_delay.count() * backoff_multiplier));
    
    // Cap the delay at 1 hour
    if (queued_email.retry_delay > std::chrono::hours(1)) {
        queued_email.retry_delay = std::chrono::hours(1);
    }
}

bool EmailQueue::comparePriority(const QueuedEmail& a, const QueuedEmail& b) {
    // Higher priority values come first
    if (a.priority != b.priority) {
        return a.priority < b.priority;
    }
    
    // For same priority, older emails come first (FIFO)
    return a.queued_at > b.queued_at;
}

} // namespace ssmtp_mailer
