# Advanced Features

The ssmtp-mailer library provides a comprehensive set of advanced features for enterprise-grade email sending applications.

## Table of Contents

1. [Rate Limiting and Backoff](#rate-limiting-and-backoff)
2. [Webhook Event Processing](#webhook-event-processing)
3. [Template Management](#template-management)
4. [Analytics and Reporting](#analytics-and-reporting)
5. [Advanced Configuration](#advanced-configuration)
6. [Performance Optimization](#performance-optimization)
7. [Security Features](#security-features)
8. [Monitoring and Alerting](#monitoring-and-alerting)

## Rate Limiting and Backoff

### Overview

Rate limiting prevents API quota exhaustion and ensures optimal performance by controlling the rate of requests to email service providers.

### Supported Strategies

- **Fixed Window**: Simple time-based limits (e.g., 100 emails per minute)
- **Sliding Window**: More accurate rate limiting with overlapping time windows
- **Token Bucket**: Burst-friendly rate limiting with token refill
- **Leaky Bucket**: Smooth, consistent rate limiting

### Configuration

```ini
[api:sendgrid]
# Rate limiting configuration
rate_limit_strategy = FIXED_WINDOW
max_requests_per_second = 100
max_requests_per_minute = 6000
max_requests_per_hour = 360000
burst_limit = 1000
```

### Usage Example

```cpp
#include "ssmtp-mailer/rate_limiter.hpp"

// Create rate limiter
ssmtp_mailer::RateLimitConfig config;
config.max_requests_per_second = 100;
config.strategy = ssmtp_mailer::RateLimitStrategy::FIXED_WINDOW;

auto rate_limiter = std::make_shared<ssmtp_mailer::RateLimiter>(config);

// Check before sending
if (rate_limiter->isAllowed()) {
    rate_limiter->recordRequest();
    // Send email...
} else {
    // Wait or use fallback
    rate_limiter->waitIfLimited();
}
```

### Provider-Specific Limits

| Provider | Default Rate | Burst Limit | Strategy | Auth Method |
|----------|--------------|-------------|----------|-------------|
| SendGrid | 100/sec | 1000 | Fixed Window | API Key |
| Mailgun | 5/sec (free) | 100 | Fixed Window | API Key |
| Amazon SES | 14/sec | 50 | Fixed Window | API Key |
| ProtonMail | 100/hour | 1000/hour | Fixed Window | OAuth2 |
| Zoho Mail | 100/day | 1000/day | Fixed Window | OAuth2 |
| Fastmail | 100/hour | 1000/hour | Fixed Window | OAuth2 |

## Webhook Event Processing

### Overview

Webhooks provide real-time delivery notifications, engagement tracking, and bounce handling from email service providers.

### Supported Event Types

- **DELIVERED**: Email successfully delivered
- **BOUNCED**: Email bounced (hard/soft)
- **DROPPED**: Email dropped by provider
- **OPENED**: Email opened by recipient
- **CLICKED**: Link clicked in email
- **UNSUBSCRIBED**: Recipient unsubscribed
- **SPAM_REPORT**: Marked as spam
- **DEFERRED**: Delivery deferred

### Configuration

```ini
[api:global]
# Webhook settings
enable_webhooks = true
webhook_timeout = 30
webhook_max_retries = 3
webhook_retry_delay = 5
webhook_verify_ssl = true
webhook_secret_required = true
```

### Usage Example

```cpp
#include "ssmtp-mailer/webhook_handler.hpp"

// Create webhook processor
ssmtp_mailer::WebhookConfig config;
config.endpoint = "https://yourdomain.com/webhooks/email";
config.secret = "your_webhook_secret";

auto processor = std::make_shared<ssmtp_mailer::WebhookEventProcessor>(config);

// Register callbacks
processor->registerCallback(ssmtp_mailer::WebhookEventType::DELIVERED, 
    [](const ssmtp_mailer::WebhookEvent& event) {
        std::cout << "Email delivered: " << event.message_id << std::endl;
    });

processor->registerCallback(ssmtp_mailer::WebhookEventType::OPENED,
    [](const ssmtp_mailer::WebhookEvent& event) {
        std::cout << "Email opened: " << event.message_id << std::endl;
    });

// Process webhook payload
std::string payload = "..."; // Raw webhook data
std::map<std::string, std::string> headers = {...};
processor->processWebhook(payload, headers, "sendgrid");
```

### Provider-Specific Webhooks

#### SendGrid
- **Endpoint**: `/webhook`
- **Authentication**: Signature verification
- **Format**: JSON array of events

#### Mailgun
- **Endpoint**: `/webhook`
- **Authentication**: Basic auth or signature
- **Format**: Form-encoded data

#### Amazon SES
- **Endpoint**: SNS topic subscription
- **Authentication**: AWS signature verification
- **Format**: JSON with SNS wrapper

## Template Management

### Overview

Template management provides dynamic email content generation with variable substitution, conditional logic, and reusable components.

### Supported Template Engines

- **Simple**: Basic variable substitution (`{{variable}}`)
- **Handlebars**: Advanced templating with conditionals and loops

### Template Structure

```cpp
ssmtp_mailer::EmailTemplate template;
template.name = "welcome_email";
template.subject_template = "Welcome to {{company_name}}, {{user_name}}!";
template.text_body_template = "Hello {{user_name}},\n\nWelcome to {{company_name}}!";
template.html_body_template = "<h1>Welcome {{user_name}}!</h1><p>Welcome to {{company_name}}!</p>";
template.category = "onboarding";
template.version = "1.0";

// Add variables
template.variables.push_back(ssmtp_mailer::TemplateVariable("user_name", "", ssmtp_mailer::TemplateVariableType::STRING, true));
template.variables.push_back(ssmtp_mailer::TemplateVariable("company_name", "Default Company", ssmtp_mailer::TemplateVariableType::STRING, false));
```

### Usage Example

```cpp
#include "ssmtp-mailer/template_manager.hpp"

// Create template manager
auto engine = ssmtp_mailer::TemplateFactory::createEngine("Simple");
auto manager = std::make_shared<ssmtp_mailer::TemplateManager>(engine);

// Add template
manager->addTemplate(template);

// Create context
ssmtp_mailer::TemplateContext context;
context.addVariable("user_name", "John Doe");
context.addVariable("company_name", "TechCorp");

// Render template
auto email = manager->renderTemplate("welcome_email", context, 
                                   "welcome@techcorp.com", {"user@example.com"});
```

### Template Categories

- **onboarding**: Welcome emails, account setup
- **marketing**: Newsletters, promotions
- **transactional**: Order confirmations, receipts
- **notifications**: System alerts, updates
- **support**: Help requests, responses

## Analytics and Reporting

### Overview

Comprehensive analytics provide insights into email performance, delivery rates, engagement metrics, and system health.

### Metrics Tracked

#### Email Metrics
- **Sent**: Total emails sent
- **Delivered**: Successfully delivered emails
- **Opened**: Emails opened by recipients
- **Clicked**: Links clicked in emails
- **Bounced**: Hard and soft bounces
- **Dropped**: Emails dropped by providers
- **Spam**: Marked as spam
- **Unsubscribed**: Recipient unsubscribes

#### Performance Metrics
- **Delivery Rate**: Percentage of emails delivered
- **Open Rate**: Percentage of emails opened
- **Click Rate**: Percentage of emails clicked
- **Bounce Rate**: Percentage of emails bounced
- **Response Time**: API response times
- **Retry Count**: Number of retry attempts

#### Provider Metrics
- **Success Rate**: Per-provider success rates
- **Failure Rate**: Per-provider failure rates
- **Performance**: Response times and throughput

### Configuration

```ini
[api:global]
# Analytics settings
analytics_enabled = true
analytics_retention_days = 90
analytics_flush_interval = 60
analytics_max_events_in_memory = 10000
```

### Usage Example

```cpp
#include "ssmtp-mailer/analytics.hpp"

// Create analytics manager
ssmtp_mailer::AnalyticsConfig config;
config.enable_tracking = true;
config.enable_persistence = true;
config.retention_days = 90;

auto analytics = std::make_shared<ssmtp_mailer::AnalyticsManager>(config);

// Track events
analytics->trackEmailSent("msg_123", "sendgrid", "sender@domain.com", 
                         "recipient@example.com", "Subject");
analytics->trackEmailDelivered("msg_123", "sendgrid");
analytics->trackEmailOpened("msg_123", "sendgrid");

// Get metrics
auto metrics = analytics->getCurrentMetrics();
std::cout << "Total sent: " << metrics.total_emails_sent << std::endl;
std::cout << "Total delivered: " << metrics.total_emails_delivered << std::endl;

// Generate reports
auto report = analytics->generateReport("summary", "json", start_time, end_time);
```

### Report Types

- **Summary**: High-level metrics overview
- **Detailed**: Comprehensive event analysis
- **Performance**: Response time and throughput analysis
- **Provider**: Provider-specific performance metrics
- **Custom**: User-defined report configurations

### Export Formats

- **JSON**: Machine-readable format for APIs
- **CSV**: Spreadsheet-compatible format
- **HTML**: Web-friendly formatted reports

## Advanced Configuration

### Provider-Specific Settings

#### SendGrid
```ini
[api:sendgrid]
enable_tracking = true
enable_analytics = true
default_template = welcome_email
custom_headers = {
    "X-Custom-Header" = "CustomValue"
}
```

#### Mailgun
```ini
[api:mailgun]
domain = yourdomain.com
region = us
enable_tracking = true
custom_headers = {
    "domain" = "yourdomain.com"
}
```

#### Amazon SES
```ini
[api:amazon-ses]
region = us-east-1
ses_configuration_set = your-config-set-name
enable_analytics = false
custom_headers = {
    "region" = "us-east-1"
}
```

### Global Settings

```ini
[api:global]
# Connection settings
connection_pool_size = 10
keep_alive_timeout = 60
max_connections_per_host = 100
enable_compression = true

# Fallback configuration
enable_smtp_fallback = true
smtp_fallback_priority = 1
api_fallback_priority = 2
max_fallback_attempts = 3

# Security settings
enable_signature_verification = true
webhook_secret_required = true
api_key_rotation_days = 90
```

## Performance Optimization

### Connection Pooling

- **Pool Size**: Configurable connection pool size
- **Keep-Alive**: Persistent connections for better performance
- **Host Limits**: Maximum connections per host
- **Compression**: HTTP compression support

### Batch Processing

- **Batch Size**: Configurable batch sizes per provider
- **Parallel Processing**: Concurrent batch processing
- **Rate Limiting**: Batch-aware rate limiting
- **Error Handling**: Partial batch failure handling

### Caching

- **Template Cache**: In-memory template caching
- **Configuration Cache**: Provider configuration caching
- **Rate Limit Cache**: Rate limit state caching
- **Connection Cache**: HTTP connection reuse

### Asynchronous Processing

- **Non-blocking**: Asynchronous email sending
- **Background Processing**: Background analytics processing
- **Event Queuing**: Webhook event queuing
- **Callback Support**: Non-blocking event callbacks

## Security Features

### Authentication

- **API Key Management**: Secure API key storage
- **Signature Verification**: Webhook signature validation
- **OAuth2 Support**: OAuth2 authentication flows
- **Credential Rotation**: Automatic credential rotation

### Data Protection

- **SSL/TLS**: Encrypted communications
- **Certificate Verification**: SSL certificate validation
- **Data Encryption**: Sensitive data encryption
- **Access Control**: Role-based access control

### Audit Logging

- **Event Logging**: Comprehensive event logging
- **Access Logs**: Authentication and access logs
- **Change Tracking**: Configuration change tracking
- **Compliance**: GDPR and SOC2 compliance support

## Monitoring and Alerting

### Health Checks

- **Provider Health**: Provider availability monitoring
- **API Endpoints**: Endpoint health checking
- **Rate Limits**: Rate limit status monitoring
- **Error Rates**: Error rate monitoring

### Alerting

```ini
[api:global]
# Alerting configuration
enable_alerts = true
alert_on_rate_limit = true
alert_on_auth_failure = true
alert_on_connection_error = true
alert_threshold_percentage = 80
```

### Metrics Dashboard

- **Real-time Metrics**: Live performance metrics
- **Historical Data**: Trend analysis and reporting
- **Provider Comparison**: Provider performance comparison
- **Custom Dashboards**: User-defined metric views

### Integration

- **Prometheus**: Metrics export for Prometheus
- **Grafana**: Dashboard integration
- **Slack**: Alert notifications
- **Email**: Alert email notifications
- **Webhooks**: Custom alert webhooks

## Best Practices

### Rate Limiting

1. **Monitor Quotas**: Track API usage and quotas
2. **Implement Backoff**: Use exponential backoff for retries
3. **Queue Management**: Queue emails when rate limited
4. **Provider Rotation**: Rotate between providers

### Template Management

1. **Version Control**: Version your email templates
2. **Testing**: Test templates with various data
3. **Localization**: Support multiple languages
4. **Accessibility**: Ensure email accessibility

### Analytics

1. **Data Retention**: Configure appropriate retention periods
2. **Privacy Compliance**: Ensure GDPR compliance
3. **Real-time Monitoring**: Monitor metrics in real-time
4. **Alert Thresholds**: Set appropriate alert thresholds

### Security

1. **Credential Management**: Use secure credential storage
2. **Access Control**: Implement least-privilege access
3. **Audit Logging**: Enable comprehensive audit logging
4. **Regular Updates**: Keep dependencies updated

## Troubleshooting

### Common Issues

#### Rate Limiting
- **Symptoms**: 429 errors, slow sending
- **Solutions**: Implement backoff, reduce sending rate

#### Webhook Failures
- **Symptoms**: Missing delivery notifications
- **Solutions**: Check endpoint availability, verify signatures

#### Template Errors
- **Symptoms**: Rendering failures, missing variables
- **Solutions**: Validate templates, check variable definitions

#### Analytics Issues
- **Symptoms**: Missing metrics, incorrect data
- **Solutions**: Check configuration, verify event tracking

### Debug Mode

```ini
[api:global]
# Debug settings
log_api_calls = true
log_response_bodies = true
log_rate_limiting = true
log_webhook_events = true
```

### Performance Tuning

1. **Connection Pooling**: Adjust pool sizes based on load
2. **Batch Sizes**: Optimize batch sizes for your use case
3. **Rate Limits**: Configure appropriate rate limits
4. **Caching**: Enable relevant caching options

## OAuth2 Authentication

### Overview

OAuth2 authentication provides enhanced security for email providers that support it. Instead of storing passwords or API keys, OAuth2 uses secure tokens that can be refreshed and revoked.

### Supported OAuth2 Providers

- **ProtonMail** - Privacy-focused email service
- **Zoho Mail** - Business email platform
- **Fastmail** - Professional email hosting

### OAuth2 Flow

1. **Authorization Request**: User authorizes the application
2. **Token Generation**: Provider generates access token
3. **Token Usage**: Application uses token for API calls
4. **Token Refresh**: Tokens are refreshed before expiration

### Configuration

```ini
[api:protonmail]
enabled = true
provider = PROTONMAIL
oauth2_token = your_oauth2_token_here
base_url = https://api.protonmail.ch
sender_email = your-email@protonmail.com

[api:zoho-mail]
enabled = true
provider = ZOHO_MAIL
oauth2_token = your_oauth2_token_here
base_url = https://api.zoho.com
sender_email = your-email@yourdomain.com

[api:fastmail]
enabled = true
provider = FASTMAIL
oauth2_token = your_oauth2_token_here
base_url = https://api.fastmail.com
sender_email = your-email@fastmail.com
```

### Token Management

```cpp
#include "ssmtp-mailer/api_client.hpp"

// OAuth2 token validation
bool isValidToken(const std::string& token) {
    // Check token format and expiration
    return !token.empty() && token.length() > 20;
}

// Token refresh handling
void refreshTokenIfNeeded(ssmtp_mailer::APIClientConfig& config) {
    // Use OAuth2 helper tools to refresh expired tokens
    // This is typically handled automatically by the helper tools
}
```

### Security Benefits

- **No Password Storage**: Tokens are used instead of passwords
- **Limited Scope**: Tokens have specific permissions
- **Revocable**: Tokens can be revoked if compromised
- **Time-Limited**: Tokens expire automatically
- **Refreshable**: New tokens can be generated without re-authorization

## Conclusion

The advanced features of ssmtp-mailer provide enterprise-grade capabilities for:

- **Scalability**: Rate limiting and connection pooling
- **Reliability**: Fallback mechanisms and retry logic
- **Monitoring**: Comprehensive analytics and alerting
- **Flexibility**: Template management and webhook processing
- **Security**: Authentication and data protection
- **Modern Authentication**: OAuth2 support for enhanced security

These features make ssmtp-mailer suitable for production environments requiring high performance, reliability, and observability.
