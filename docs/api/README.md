# API-Based Email Sending

## Overview

ssmtp-mailer now supports sending emails via REST APIs in addition to traditional SMTP. This provides several advantages:

- **Higher reliability** - API services have better deliverability
- **Faster sending** - No need to establish SMTP connections
- **Better tracking** - Built-in analytics and delivery tracking
- **Scalability** - Handle high-volume sending without connection limits
- **Modern integration** - Easy to integrate with web applications

## Supported Providers

### Currently Implemented

1. **SendGrid** - Popular email delivery service with excellent deliverability
2. **Mailgun** - Email API service focused on transactional emails
3. **Amazon SES** - AWS Simple Email Service for high-volume sending

### Planned Support

4. **Postmark** - Transactional email service
5. **SparkPost** - Email delivery platform
6. **Mailjet** - Email marketing and transactional service

## Quick Start

### 1. Configuration

Create an API configuration file (`api-config.conf`):

```ini
[api:sendgrid]
enabled = true
provider = SENDGRID
api_key = your_sendgrid_api_key_here
sender_email = your-verified-sender@yourdomain.com
sender_name = Your Company Name
```

### 2. Command Line Usage

```bash
# Send email via API
ssmtp-mailer send-api --provider sendgrid \
  --from sender@yourdomain.com \
  --to recipient@example.com \
  --subject "Test Email" \
  --body "Hello from API!"

# Test API connection
ssmtp-mailer test-api --provider sendgrid
```

### 3. Programmatic Usage

```cpp
#include "ssmtp-mailer/unified_mailer.hpp"

// Create unified mailer configuration
ssmtp_mailer::UnifiedMailerConfig config;
config.default_method = ssmtp_mailer::SendMethod::API;

// Add API provider configuration
ssmtp_mailer::APIClientConfig sendgrid_config;
sendgrid_config.provider = ssmtp_mailer::APIProvider::SENDGRID;
sendgrid_config.auth.api_key = "your_api_key";
sendgrid_config.sender_email = "sender@yourdomain.com";

config.api_configs["sendgrid"] = sendgrid_config;

// Create unified mailer
ssmtp_mailer::UnifiedMailer mailer(config);

// Send email
ssmtp_mailer::Email email("sender@yourdomain.com", "recipient@example.com", 
                         "Subject", "Body");
auto result = mailer.sendEmail(email);

if (result.success) {
    std::cout << "Email sent via " << result.provider_name << std::endl;
    std::cout << "Message ID: " << result.message_id << std::endl;
} else {
    std::cerr << "Failed: " << result.error_message << std::endl;
}
```

## Configuration Details

### API Provider Configuration

Each API provider has its own configuration section:

```ini
[api:provider_name]
enabled = true                    # Enable/disable this provider
provider = PROVIDER_TYPE          # Provider type (SENDGRID, MAILGUN, etc.)
api_key = your_api_key           # API key for authentication
base_url = https://api.example.com  # Base URL for API calls
endpoint = /v1/send              # API endpoint for sending emails
sender_email = sender@domain.com # Verified sender email
sender_name = Sender Name        # Sender display name
timeout_seconds = 30             # Request timeout
verify_ssl = true                # SSL certificate verification
enable_tracking = true           # Enable email tracking
```

### Authentication Methods

Different providers support different authentication methods:

- **API Key** - Most common (SendGrid, Mailgun)
- **OAuth2** - For services that support it
- **Bearer Token** - For token-based authentication
- **Basic Auth** - Username/password authentication

### Global Settings

```ini
[api:global]
default_timeout = 30             # Default timeout for all requests
max_retries = 3                 # Maximum retry attempts
retry_delay_seconds = 5         # Delay between retries
requests_per_second = 10        # Rate limiting
log_api_calls = true            # Log all API calls
```

## Provider-Specific Setup

### SendGrid

1. Create account at [sendgrid.com](https://sendgrid.com)
2. Generate API key in Settings → API Keys
3. Verify sender email address
4. Configure webhook settings (optional)

```ini
[api:sendgrid]
enabled = true
provider = SENDGRID
api_key = SG.your_api_key_here
sender_email = your-verified-sender@yourdomain.com
enable_tracking = true
```

### Mailgun

1. Create account at [mailgun.com](https://mailgun.com)
2. Get API key from Dashboard → API Keys
3. Verify domain ownership
4. Configure webhook endpoints

```ini
[api:mailgun]
enabled = true
provider = MAILGUN
api_key = key-your_api_key_here
domain = yourdomain.com
region = us  # or eu
```

### Amazon SES

1. Set up AWS account
2. Create IAM user with SES permissions
3. Generate access key and secret
4. Verify sender email address
5. Move out of sandbox mode

```ini
[api:amazon-ses]
enabled = true
provider = AMAZON_SES
api_key = your_aws_access_key_id
api_secret = your_aws_secret_access_key
region = us-east-1
```

## Advanced Features

### Automatic Fallback

The unified mailer can automatically fall back to SMTP if API sending fails:

```cpp
config.enable_fallback = true;
config.default_method = ssmtp_mailer::SendMethod::AUTO;

// This will try API first, then SMTP if API fails
auto result = mailer.sendEmail(email);
```

### Batch Sending

Send multiple emails efficiently:

```cpp
std::vector<ssmtp_mailer::Email> emails;
// ... populate emails vector

auto results = mailer.sendBatch(emails, ssmtp_mailer::SendMethod::API);
for (const auto& result : results) {
    if (result.success) {
        std::cout << "Email sent: " << result.message_id << std::endl;
    }
}
```

### Provider Selection

Automatically select the best provider based on email characteristics:

```cpp
// The unified mailer will automatically select the best available provider
auto result = mailer.sendViaAPI(email);

// Or specify a particular provider
auto result = mailer.sendViaAPI(email, "sendgrid");
```

### Monitoring and Statistics

Track sending performance:

```cpp
auto stats = mailer.getStatistics();
std::cout << "API success: " << stats["api_success"] << std::endl;
std::cout << "API failures: " << stats["api_failure"] << std::endl;
std::cout << "Fallbacks: " << stats["fallbacks"] << std::endl;
```

## Error Handling

### Common Error Types

- **Authentication errors** - Invalid API keys or credentials
- **Rate limiting** - Exceeded sending quotas
- **Validation errors** - Invalid email addresses or content
- **Network errors** - Connection timeouts or failures

### Error Handling Example

```cpp
auto result = mailer.sendEmail(email);
if (!result.success) {
    std::cerr << "Failed to send email: " << result.error_message << std::endl;
    
    // Check if we should retry
    if (result.retry_count < config.max_retries) {
        std::cout << "Retrying..." << std::endl;
        // The unified mailer handles retries automatically
    }
}
```

## Performance Considerations

### Rate Limiting

Most API providers have rate limits:

- **SendGrid**: 100 emails/second
- **Mailgun**: 5 emails/second (free), 1000 emails/second (paid)
- **Amazon SES**: 14 emails/second (default), configurable

### Connection Pooling

The HTTP client automatically handles connection reuse and pooling for optimal performance.

### Batch Processing

For high-volume sending, use batch operations when possible:

```cpp
// More efficient than individual sends
auto results = mailer.sendBatch(emails);
```

## Security Best Practices

1. **Store API keys securely** - Use environment variables or secure key management
2. **Verify sender addresses** - Always verify sender emails with providers
3. **Use HTTPS** - All API communications use TLS encryption
4. **Monitor usage** - Track API usage to detect anomalies
5. **Rotate keys** - Periodically rotate API keys

## Troubleshooting

### Common Issues

1. **Authentication failed**
   - Check API key validity
   - Verify sender email is verified
   - Check account status

2. **Rate limiting**
   - Reduce sending frequency
   - Implement exponential backoff
   - Upgrade account tier

3. **Delivery failures**
   - Check recipient email validity
   - Verify domain reputation
   - Review provider logs

### Debug Mode

Enable verbose logging to troubleshoot issues:

```bash
ssmtp-mailer --verbose test-api --provider sendgrid
```

### Provider Logs

Most providers offer detailed logs and analytics:
- **SendGrid**: Activity → Mail
- **Mailgun**: Logs → Events
- **Amazon SES**: Sending Statistics

## Migration from SMTP

### Gradual Migration

1. Start with non-critical emails
2. Monitor delivery rates and performance
3. Gradually increase API usage
4. Keep SMTP as fallback

### Configuration Migration

```ini
# Old SMTP-only configuration
[smtp]
server = smtp.gmail.com
port = 587
username = user@gmail.com
password = app_password

# New unified configuration
[smtp]
# ... existing SMTP config ...

[api:sendgrid]
# ... new API config ...
```

## Future Enhancements

- **Webhook support** - Real-time delivery notifications
- **Template management** - Email template support
- **Advanced analytics** - Detailed sending statistics
- **Multi-provider load balancing** - Distribute load across providers
- **A/B testing** - Test different sending strategies

## Support

For issues and questions:

1. Check provider documentation
2. Review error logs
3. Test with simple examples
4. Contact support with detailed error information

## Examples

See the `examples/` directory for complete working examples of API-based email sending.
