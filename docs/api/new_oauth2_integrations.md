# New OAuth2 API Integrations

## Overview

ssmtp-mailer now supports three new email providers with OAuth2 authentication:

- **ProtonMail** - Privacy-focused email service
- **Zoho Mail** - Business email and collaboration platform  
- **Fastmail** - Professional email hosting service

These integrations provide enhanced security through OAuth2 authentication and modern API-based email sending capabilities.

## ProtonMail Integration

### About ProtonMail

ProtonMail is a privacy-focused email service that provides end-to-end encryption and is based in Switzerland. It's ideal for users who prioritize security and privacy.

### Features

- **OAuth2 Authentication** - Secure token-based authentication
- **Privacy-Focused** - End-to-end encryption
- **Swiss-Based** - Strong privacy laws and regulations
- **Professional API** - RESTful API for programmatic access

### Setup Process

1. **Create ProtonMail Account**
   - Visit [protonmail.com](https://protonmail.com)
   - Choose a plan (Free or Paid)
   - Complete account verification

2. **Enable API Access**
   - Go to Settings → Security → API Access
   - Enable API access for your account
   - Note: API access may require a paid plan

3. **Generate OAuth2 Token**
   ```bash
   # Use our OAuth2 helper tool
   python3 tools/oauth2-helper/python/protonmail-oauth2-helper.py
   ```

4. **Configuration**
   ```ini
   [api:protonmail]
   enabled = true
   provider = PROTONMAIL
   oauth2_token = your_oauth2_token_here
   base_url = https://api.protonmail.ch
   sender_email = your-email@protonmail.com
   sender_name = Your Name
   timeout_seconds = 30
   verify_ssl = true
   ```

### Usage Example

```cpp
#include "ssmtp-mailer/api_client.hpp"

// Create ProtonMail API client configuration
ssmtp_mailer::APIClientConfig config;
config.provider = ssmtp_mailer::APIProvider::PROTONMAIL;
config.auth.oauth2_token = "your_oauth2_token";
config.request.base_url = "https://api.protonmail.ch";
config.sender_email = "your-email@protonmail.com";
config.sender_name = "Your Name";

// Create client
auto client = std::make_shared<ssmtp_mailer::ProtonMailAPIClient>(config);

// Test connection
if (client->testConnection()) {
    std::cout << "ProtonMail connection successful!" << std::endl;
}

// Send email
ssmtp_mailer::Email email;
email.from = "your-email@protonmail.com";
email.to = {"recipient@example.com"};
email.subject = "Test from ProtonMail";
email.body = "Hello from ProtonMail via ssmtp-mailer!";

auto result = client->sendEmail(email);
if (result.success) {
    std::cout << "Email sent! Message ID: " << result.message_id << std::endl;
}
```

## Zoho Mail Integration

### About Zoho Mail

Zoho Mail is a business email solution that provides professional email hosting with collaboration tools. It's ideal for businesses and organizations.

### Features

- **OAuth2 Authentication** - Secure enterprise authentication
- **Business-Focused** - Professional email hosting
- **Collaboration Tools** - Integrated with Zoho ecosystem
- **Custom Domains** - Use your own domain for email

### Setup Process

1. **Create Zoho Account**
   - Visit [zoho.com](https://zoho.com)
   - Sign up for Zoho Mail
   - Choose a plan (Free or Paid)

2. **Set Up OAuth2 Application**
   - Go to [Zoho Developer Console](https://api-console.zoho.com/)
   - Create a new client
   - Configure redirect URIs
   - Note client ID and secret

3. **Generate OAuth2 Token**
   ```bash
   # Use our OAuth2 helper tool
   python3 tools/oauth2-helper/python/zoho-mail-oauth2-helper.py
   ```

4. **Configuration**
   ```ini
   [api:zoho-mail]
   enabled = true
   provider = ZOHO_MAIL
   oauth2_token = your_oauth2_token_here
   base_url = https://api.zoho.com
   sender_email = your-email@yourdomain.com
   sender_name = Your Name
   enable_tracking = true
   timeout_seconds = 30
   verify_ssl = true
   ```

### Usage Example

```cpp
#include "ssmtp-mailer/api_client.hpp"

// Create Zoho Mail API client configuration
ssmtp_mailer::APIClientConfig config;
config.provider = ssmtp_mailer::APIProvider::ZOHO_MAIL;
config.auth.oauth2_token = "your_oauth2_token";
config.request.base_url = "https://api.zoho.com";
config.sender_email = "your-email@yourdomain.com";
config.sender_name = "Your Name";
config.enable_tracking = true;

// Create client
auto client = std::make_shared<ssmtp_mailer::ZohoMailAPIClient>(config);

// Test connection
if (client->testConnection()) {
    std::cout << "Zoho Mail connection successful!" << std::endl;
}

// Send email with tracking
ssmtp_mailer::Email email;
email.from = "your-email@yourdomain.com";
email.to = {"recipient@example.com"};
email.subject = "Test from Zoho Mail";
email.body = "Hello from Zoho Mail via ssmtp-mailer!";
email.html_body = "<h1>Hello from Zoho Mail!</h1><p>This email supports HTML.</p>";

auto result = client->sendEmail(email);
if (result.success) {
    std::cout << "Email sent! Message ID: " << result.message_id << std::endl;
}
```

## Fastmail Integration

### About Fastmail

Fastmail is a professional email hosting service known for its speed, reliability, and excellent user experience. It's popular among power users and professionals.

### Features

- **OAuth2 Authentication** - Modern authentication standards
- **Fast & Reliable** - Optimized for performance
- **Professional Hosting** - Business-grade email service
- **Custom Domains** - Professional domain hosting

### Setup Process

1. **Create Fastmail Account**
   - Visit [fastmail.com](https://fastmail.com)
   - Choose a plan (Personal or Professional)
   - Complete account setup

2. **Enable API Access**
   - Go to Settings → Security → API Access
   - Enable API access for your account
   - Note: API access may require a paid plan

3. **Generate OAuth2 Token**
   ```bash
   # Use our OAuth2 helper tool
   python3 tools/oauth2-helper/python/fastmail-oauth2-helper.py
   ```

4. **Configuration**
   ```ini
   [api:fastmail]
   enabled = true
   provider = FASTMAIL
   oauth2_token = your_oauth2_token_here
   base_url = https://api.fastmail.com
   sender_email = your-email@fastmail.com
   sender_name = Your Name
   enable_tracking = true
   timeout_seconds = 30
   verify_ssl = true
   ```

### Usage Example

```cpp
#include "ssmtp-mailer/api_client.hpp"

// Create Fastmail API client configuration
ssmtp_mailer::APIClientConfig config;
config.provider = ssmtp_mailer::APIProvider::FASTMAIL;
config.auth.oauth2_token = "your_oauth2_token";
config.request.base_url = "https://api.fastmail.com";
config.sender_email = "your-email@fastmail.com";
config.sender_name = "Your Name";
config.enable_tracking = true;

// Create client
auto client = std::make_shared<ssmtp_mailer::FastmailAPIClient>(config);

// Test connection
if (client->testConnection()) {
    std::cout << "Fastmail connection successful!" << std::endl;
}

// Send email with attachments
ssmtp_mailer::Email email;
email.from = "your-email@fastmail.com";
email.to = {"recipient@example.com"};
email.cc = {"cc@example.com"};
email.subject = "Test from Fastmail";
email.body = "Hello from Fastmail via ssmtp-mailer!";
email.html_body = "<h1>Hello from Fastmail!</h1><p>This email supports HTML.</p>";
email.attachments = {"/path/to/document.pdf"};

auto result = client->sendEmail(email);
if (result.success) {
    std::cout << "Email sent! Message ID: " << result.message_id << std::endl;
}
```

## OAuth2 Token Management

### Token Generation

All three providers use our OAuth2 helper tools for token generation:

```bash
# List available providers
python3 tools/oauth2-helper/oauth2-helper.py --list

# Generate token for specific provider
python3 tools/oauth2-helper/oauth2-helper.py protonmail
python3 tools/oauth2-helper/oauth2-helper.py zoho-mail
python3 tools/oauth2-helper/oauth2-helper.py fastmail
```

### Token Refresh

OAuth2 tokens expire periodically. Use the helper tools to refresh them:

```bash
# Refresh expired token
python3 tools/oauth2-helper/oauth2-helper.py protonmail --refresh
```

### Security Best Practices

1. **Store tokens securely** - Use environment variables or secure storage
2. **Rotate tokens regularly** - Refresh tokens before expiration
3. **Monitor usage** - Track API usage for security
4. **Use HTTPS** - All API communications are encrypted
5. **Limit permissions** - Use minimal required OAuth2 scopes

## Rate Limits and Quotas

### ProtonMail
- **Free Plan**: 100 emails/hour
- **Paid Plans**: 1000 emails/hour
- **API Limits**: 100 requests/minute

### Zoho Mail
- **Free Plan**: 100 emails/day
- **Paid Plans**: 1000 emails/day
- **API Limits**: 100 requests/minute

### Fastmail
- **Free Plan**: 100 emails/hour
- **Paid Plans**: 1000 emails/hour
- **API Limits**: 100 requests/minute

## Troubleshooting

### Common Issues

1. **OAuth2 Token Expired**
   - Use helper tool to refresh token
   - Check token expiration time
   - Verify OAuth2 application settings

2. **Authentication Failed**
   - Verify OAuth2 token validity
   - Check provider account status
   - Ensure API access is enabled

3. **Rate Limiting**
   - Reduce sending frequency
   - Check account quotas
   - Upgrade plan if needed

### Debug Mode

Enable verbose logging for troubleshooting:

```bash
# Test API connection with verbose output
ssmtp-mailer --verbose test-api --provider protonmail
ssmtp-mailer --verbose test-api --provider zoho-mail
ssmtp-mailer --verbose test-api --provider fastmail
```

### Provider Support

- **ProtonMail**: [Support Portal](https://protonmail.com/support/)
- **Zoho Mail**: [Help Center](https://help.zoho.com/)
- **Fastmail**: [Support](https://www.fastmail.com/help/)

## Migration Guide

### From SMTP to OAuth2 API

1. **Generate OAuth2 Token**
   - Use our helper tools
   - Follow provider-specific setup

2. **Update Configuration**
   - Replace SMTP settings with API config
   - Update authentication method

3. **Test Integration**
   - Verify connection
   - Send test emails
   - Monitor delivery rates

4. **Gradual Migration**
   - Start with non-critical emails
   - Monitor performance
   - Keep SMTP as fallback

### Configuration Comparison

```ini
# Old SMTP Configuration
[smtp]
server = smtp.protonmail.com
port = 587
username = your-email@protonmail.com
password = your_password

# New OAuth2 API Configuration
[api:protonmail]
enabled = true
provider = PROTONMAIL
oauth2_token = your_oauth2_token
base_url = https://api.protonmail.ch
sender_email = your-email@protonmail.com
```

## Conclusion

The new OAuth2 integrations provide:

- **Enhanced Security** - Token-based authentication
- **Modern APIs** - RESTful interfaces
- **Better Reliability** - Improved deliverability
- **Professional Features** - Business-grade capabilities

These integrations make ssmtp-mailer a comprehensive solution for both traditional SMTP and modern API-based email sending, with strong support for OAuth2 authentication across multiple providers.

For more information, see the [OAuth2 Overview](../oauth2/oauth2-overview.md) and [Provider-Specific Setup Guides](../oauth2/README.md).
