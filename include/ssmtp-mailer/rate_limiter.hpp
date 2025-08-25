#pragma once

#include <chrono>
#include <map>
#include <string>
#include <mutex>
#include <atomic>

namespace ssmtp_mailer {

/**
 * @brief Rate limiting strategy
 */
enum class RateLimitStrategy {
    FIXED_WINDOW,      // Fixed time window (e.g., 100 emails per minute)
    SLIDING_WINDOW,    // Sliding time window
    TOKEN_BUCKET,      // Token bucket algorithm
    LEAKY_BUCKET       // Leaky bucket algorithm
};

/**
 * @brief Rate limit configuration
 */
struct RateLimitConfig {
    int max_requests_per_second;
    int max_requests_per_minute;
    int max_requests_per_hour;
    int burst_limit;
    std::chrono::milliseconds window_size;
    RateLimitStrategy strategy;
    
    RateLimitConfig() : max_requests_per_second(10), max_requests_per_minute(600), 
                       max_requests_per_hour(36000), burst_limit(100),
                       window_size(std::chrono::milliseconds(1000)), 
                       strategy(RateLimitStrategy::FIXED_WINDOW) {}
};

/**
 * @brief Rate limiter for API providers
 */
class RateLimiter {
public:
    /**
     * @brief Constructor
     * @param config Rate limiting configuration
     */
    explicit RateLimiter(const RateLimitConfig& config);
    
    /**
     * @brief Check if request is allowed
     * @return true if allowed, false if rate limited
     */
    bool isAllowed();
    
    /**
     * @brief Record a request
     */
    void recordRequest();
    
    /**
     * @brief Wait if rate limited (with exponential backoff)
     * @return true if waited successfully, false if timeout
     */
    bool waitIfLimited();
    
    /**
     * @brief Get current rate limit status
     * @return Map of current usage statistics
     */
    std::map<std::string, int> getStatus() const;
    
    /**
     * @brief Reset rate limiter
     */
    void reset();
    
    /**
     * @brief Update configuration
     * @param config New configuration
     */
    void updateConfig(const RateLimitConfig& config);

private:
    RateLimitConfig config_;
    mutable std::mutex mutex_;
    
    // Request counters
    std::atomic<int> requests_this_second_;
    std::atomic<int> requests_this_minute_;
    std::atomic<int> requests_this_hour_;
    std::atomic<int> total_requests_;
    
    // Timing windows
    std::chrono::steady_clock::time_point window_start_;
    std::chrono::steady_clock::time_point minute_start_;
    std::chrono::steady_clock::time_point hour_start_;
    
    // Backoff tracking
    int consecutive_failures_;
    std::chrono::steady_clock::time_point last_failure_time_;
    
    // Helper methods
    void updateWindows();
    bool isFixedWindowAllowed();
    bool isSlidingWindowAllowed();
    bool isTokenBucketAllowed();
    bool isLeakyBucketAllowed();
    std::chrono::milliseconds calculateBackoffDelay();
};

/**
 * @brief Provider-specific rate limiter factory
 */
class RateLimiterFactory {
public:
    /**
     * @brief Create rate limiter for specific provider
     * @param provider Provider name
     * @return Rate limiter instance
     */
    static std::shared_ptr<RateLimiter> createForProvider(const std::string& provider);
    
    /**
     * @brief Get default configuration for provider
     * @param provider Provider name
     * @return Default rate limit configuration
     */
    static RateLimitConfig getDefaultConfig(const std::string& provider);
    
    /**
     * @brief Get all supported providers
     * @return Vector of provider names
     */
    static std::vector<std::string> getSupportedProviders();

private:
    static std::map<std::string, RateLimitConfig> default_configs_;
    static void initializeDefaultConfigs();
};

} // namespace ssmtp_mailer
