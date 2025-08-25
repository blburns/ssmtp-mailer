# OAuth2 Authentication Documentation

Welcome to the comprehensive OAuth2 authentication documentation for ssmtp-mailer. This section covers all supported email providers and their OAuth2 setup processes.

## 📚 Available Guides

### 🔐 Core OAuth2 Concepts
- **[OAuth2 Overview](oauth2-overview.md)** - Understanding OAuth2 authentication
- **[OAuth2 Security Best Practices](oauth2-security.md)** - Security considerations and best practices

### 📧 Email Provider Guides

#### Personal & Business Email
- **[Gmail OAuth2 Setup](gmail-oauth2-setup.md)** - Complete Gmail and Google Workspace setup
- **[Office 365 OAuth2 Setup](office365-oauth2-setup.md)** - Microsoft 365 and Office 365 authentication
- **[ProtonMail OAuth2 Setup](protonmail-oauth2-setup.md)** - Privacy-focused email authentication
- **[Fastmail OAuth2 Setup](fastmail-oauth2-setup.md)** - Professional email hosting setup

#### Transactional Email Services
- **[SendGrid OAuth2 Setup](sendgrid-oauth2-setup.md)** - Email delivery service authentication
- **[Amazon SES OAuth2 Setup](amazon-ses-oauth2-setup.md)** - AWS Simple Email Service setup
- **[Mailgun OAuth2 Setup](mailgun-oauth2-setup.md)** - Email API service authentication

#### Business Email Solutions
- **[Zoho Mail OAuth2 Setup](zoho-mail-oauth2-setup.md)** - Business email and collaboration platform

### 🛠️ Tools & Utilities
- **[OAuth2 Helper Tools](oauth2-helper-tools.md)** - Using the built-in OAuth2 helper scripts
- **[Token Management](token-management.md)** - Managing and refreshing OAuth2 tokens
- **[Troubleshooting Guide](oauth2-troubleshooting.md)** - Common issues and solutions

## 🚀 Quick Start

### 1. Choose Your Email Provider
Select the guide for your email service from the list above.

### 2. Set Up OAuth2 Credentials
Follow the provider-specific guide to:
- Create OAuth2 application
- Get client ID and secret
- Configure redirect URIs

### 3. Use the Helper Tools
```bash
# List all available providers
python3 tools/oauth2-helper/oauth2-helper.py --list

# Launch provider-specific helper
python3 tools/oauth2-helper/oauth2-helper.py gmail
python3 tools/oauth2-helper/oauth2-helper.py office365
# ... and more
```

### 4. Configure ssmtp-mailer
Use the generated tokens in your ssmtp-mailer configuration.

## 🔧 Prerequisites

- Python 3.6+ (for helper scripts)
- Modern web browser (for web-based helpers)
- OAuth2 credentials from your email provider
- Basic understanding of OAuth2 flow

## 📖 How to Use This Documentation

1. **Start with the Overview** if you're new to OAuth2
2. **Choose your provider** from the email provider guides
3. **Follow the step-by-step setup** instructions
4. **Use the helper tools** to generate tokens
5. **Reference troubleshooting** if you encounter issues

## 🆘 Getting Help

- **Check the troubleshooting guide** for common solutions
- **Review provider-specific guides** for detailed setup
- **Use the helper tools** for automated token generation
- **Open an issue** in our repository if you need assistance

## 🔒 Security Notice

OAuth2 credentials provide access to your email account. Always:
- Keep client secrets secure
- Use HTTPS for production redirect URIs
- Regularly rotate credentials
- Monitor authentication logs
- Follow security best practices

---

*This documentation is maintained as part of the ssmtp-mailer project. For updates and contributions, please visit our [GitHub repository](https://github.com/blburns/ssmtp-mailer).*
