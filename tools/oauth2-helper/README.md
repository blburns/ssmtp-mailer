# 🔐 OAuth2 Helper Tools

This directory contains OAuth2 authentication helper tools for ssmtp-mailer, making it easy to set up authentication with popular email providers.

## 📁 Directory Structure

```
tools/oauth2-helper/
├── python/                    # Python-based OAuth2 helpers
│   ├── gmail-oauth2-helper.py    # Gmail/Google Workspace OAuth2 helper
│   └── office365-oauth2-helper.py # Office 365 OAuth2 helper
├── web/                      # Web-based OAuth2 helpers
│   └── gmail-oauth2-helper.html  # Gmail OAuth2 web interface
├── examples/                 # Example configurations and usage
├── oauth2-helper.py         # Main launcher script
└── README.md                 # This file
```

## 🚀 Quick Start

### Using the Launcher Script

The easiest way to use OAuth2 helpers is through the main launcher script:

```bash
# List available OAuth2 providers
python tools/oauth2-helper/oauth2-helper.py --list

# Launch Gmail OAuth2 helper
python tools/oauth2-helper/oauth2-helper.py gmail

# Launch Office 365 OAuth2 helper
python tools/oauth2-helper/oauth2-helper.py office365

# Launch SendGrid OAuth2 helper
python tools/oauth2-helper/oauth2-helper.py sendgrid

# Launch Amazon SES OAuth2 helper
python tools/oauth2-helper/oauth2-helper.py amazon-ses
```

### Direct Script Usage

You can also run the individual helper scripts directly:

```bash
# Gmail OAuth2 helper
python tools/oauth2-helper/python/gmail-oauth2-helper.py

# Office 365 OAuth2 helper
python tools/oauth2-helper/python/office365-oauth2-helper.py

# SendGrid OAuth2 helper
python tools/oauth2-helper/python/sendgrid-oauth2-helper.py

# Amazon SES OAuth2 helper
python tools/oauth2-helper/python/amazon-ses-oauth2-helper.py

# ProtonMail OAuth2 helper
python tools/oauth2-helper/python/protonmail-oauth2-helper.py

# Zoho Mail OAuth2 helper
python tools/oauth2-helper/python/zoho-mail-oauth2-helper.py

# Mailgun OAuth2 helper
python tools/oauth2-helper/python/mailgun-oauth2-helper.py

# Fastmail OAuth2 helper
python tools/oauth2-helper/python/fastmail-oauth2-helper.py
```

## 📧 Supported Providers

### Gmail / Google Workspace

**Features:**
- Google Service Account support
- Gmail API integration
- Automatic token refresh
- Local callback server

**Requirements:**
- Python 3.6+
- `requests` library
- Google Cloud Console project
- OAuth2 credentials

**Setup:**
1. Create a project in [Google Cloud Console](https://console.cloud.google.com/)
2. Enable Gmail API
3. Create OAuth2 credentials
4. Download client ID and secret
5. Run the helper script

### Office 365 / Microsoft Graph

**Features:**
- Microsoft Graph API support
- Office 365 integration
- Tenant-based authentication
- Modern authentication flow

**Requirements:**
- Python 3.6+
- `requests` library
- Azure AD application
- OAuth2 credentials

**Setup:**
1. Register an application in [Azure Portal](https://portal.azure.com/)
2. Configure API permissions (Mail.Send)
3. Get client ID, secret, and tenant ID
4. Run the helper script

### SendGrid

**Features:**
- SendGrid API integration
- Transactional email support
- Webhook management
- High deliverability

**Requirements:**
- Python 3.6+
- `requests` library
- SendGrid account
- OAuth2 credentials

**Setup:**
1. Create a SendGrid account
2. Enable OAuth2 in your account settings
3. Create OAuth2 application
4. Get client ID and secret
5. Run the helper script

### Amazon SES

**Features:**
- AWS SES integration
- Regional endpoint support
- High deliverability
- AWS IAM integration

**Requirements:**
- Python 3.6+
- `requests` library
- AWS account
- OAuth2 credentials

**Setup:**
1. Set up AWS account and SES
2. Configure OAuth2 application
3. Get client ID and secret
4. Specify your AWS region
5. Run the helper script

### ProtonMail

**Features:**
- ProtonMail integration
- Privacy-focused email
- End-to-end encryption
- Swiss-based security

**Requirements:**
- Python 3.6+
- `requests` library
- ProtonMail account
- OAuth2 credentials

**Setup:**
1. Create a ProtonMail account
2. Enable OAuth2 in your account settings
3. Create OAuth2 application
4. Get client ID and secret
5. Run the helper script

### Zoho Mail

**Features:**
- Zoho Mail integration
- Business email support
- API-based access
- Enterprise features

**Requirements:**
- Python 3.6+
- `requests` library
- Zoho Mail account
- OAuth2 credentials

**Setup:**
1. Set up Zoho Mail account
2. Configure OAuth2 application
3. Get client ID and secret
4. Run the helper script

### Mailgun

**Features:**
- Mailgun API integration
- Transactional email
- Email analytics
- High deliverability

**Requirements:**
- Python 3.6+
- `requests` library
- Mailgun account
- OAuth2 credentials

**Setup:**
1. Create a Mailgun account
2. Enable OAuth2 in your account settings
3. Create OAuth2 application
4. Get client ID and secret
5. Run the helper script

### Fastmail

**Features:**
- Fastmail integration
- Professional hosting
- IMAP/SMTP support
- Custom domains

**Requirements:**
- Python 3.6+
- `requests` library
- Fastmail account
- OAuth2 credentials

**Setup:**
1. Set up Fastmail account
2. Configure OAuth2 application
3. Get client ID and secret
4. Run the helper script

## 🔧 Installation

### Prerequisites

```bash
# Install required Python packages
pip install requests

# Or install from requirements file
pip install -r tools/oauth2-helper/requirements.txt
```

### System Installation

The OAuth2 helper tools are automatically included when you install ssmtp-mailer:

```bash
# Install ssmtp-mailer (includes OAuth2 tools)
sudo make install

# OAuth2 tools will be available at:
# /usr/local/bin/oauth2-helper
# /usr/local/share/ssmtp-mailer/tools/oauth2-helper/
```

## 📖 Usage Examples

### Gmail OAuth2 Setup

```bash
# Launch Gmail helper
oauth2-helper gmail

# Follow the prompts:
# 1. Enter your OAuth2 Client ID
# 2. Enter your OAuth2 Client Secret
# 3. Open the authorization URL in your browser
# 4. Sign in and grant permissions
# 5. Tokens will be saved to oauth2_tokens.json
```

### Office 365 OAuth2 Setup

```bash
# Launch Office 365 helper
oauth2-helper office365

# Follow the prompts:
# 1. Enter your OAuth2 Client ID
# 2. Enter your OAuth2 Client Secret
# 3. Enter your Tenant ID
# 4. Open the authorization URL in your browser
# 5. Sign in and grant permissions
# 6. Tokens will be saved to office365_oauth2_tokens.json

# Launch SendGrid helper
oauth2-helper sendgrid

# Follow the prompts:
# 1. Enter your OAuth2 Client ID
# 2. Enter your OAuth2 Client Secret
# 3. Open the authorization URL in your browser
# 4. Sign in and grant permissions
# 5. Tokens will be saved to sendgrid_oauth2_tokens.json

# Launch Amazon SES helper
oauth2-helper amazon-ses

# Follow the prompts:
# 1. Enter your OAuth2 Client ID
# 2. Enter your OAuth2 Client Secret
# 3. Enter your AWS region (default: us-east-1)
# 4. Open the authorization URL in your browser
# 5. Sign in and grant permissions
# 6. Tokens will be saved to amazon_ses_oauth2_tokens.json

# Launch ProtonMail helper
oauth2-helper protonmail

# Follow the prompts:
# 1. Enter your OAuth2 Client ID
# 2. Enter your OAuth2 Client Secret
# 3. Open the authorization URL in your browser
# 4. Sign in and grant permissions
# 5. Tokens will be saved to protonmail_oauth2_tokens.json

# Launch Zoho Mail helper
oauth2-helper zoho-mail

# Follow the prompts:
# 1. Enter your OAuth2 Client ID
# 2. Enter your OAuth2 Client Secret
# 3. Open the authorization URL in your browser
# 4. Sign in and grant permissions
# 5. Tokens will be saved to zoho_mail_oauth2_tokens.json

# Launch Mailgun helper
oauth2-helper mailgun

# Follow the prompts:
# 1. Enter your OAuth2 Client ID
# 2. Enter your OAuth2 Client Secret
# 3. Open the authorization URL in your browser
# 4. Sign in and grant permissions
# 5. Tokens will be saved to mailgun_oauth2_tokens.json

# Launch Fastmail helper
oauth2-helper fastmail

# Follow the prompts:
# 1. Enter your OAuth2 Client ID
# 2. Enter your OAuth2 Client Secret
# 3. Open the authorization URL in your browser
# 4. Sign in and grant permissions
# 5. Tokens will be saved to fastmail_oauth2_tokens.json
```

### Web Interface

For Gmail, you can also use the web-based interface:

```bash
# Open the HTML file in your browser
open tools/oauth2-helper/web/gmail-oauth2-helper.html

# Or serve it locally
python -m http.server 8000
# Then open http://localhost:8000/tools/oauth2-helper/web/gmail-oauth2-helper.html
```

## ⚙️ Configuration

### Token Files

The helpers generate JSON token files that can be used with ssmtp-mailer:

**Gmail tokens (oauth2_tokens.json):**
```json
{
  "access_token": "ya29.a0AfH6SMB...",
  "refresh_token": "1//04d...",
  "token_type": "Bearer",
  "expires_in": 3599
}
```

**Office 365 tokens (office365_oauth2_tokens.json):**
```json
{
  "access_token": "eyJ0eXAiOiJKV1QiLCJhbGciOiJSUzI1Ni...",
  "refresh_token": "M.R3_BAY...",
  "token_type": "Bearer",
  "expires_in": 3600
}
```

**SendGrid tokens (sendgrid_oauth2_tokens.json):**
```json
{
  "access_token": "SG.abc123...",
  "refresh_token": "refresh_token_here",
  "token_type": "Bearer",
  "expires_in": 3600
}
```

**Amazon SES tokens (amazon_ses_oauth2_tokens.json):**
```json
{
  "access_token": "access_token_here",
  "refresh_token": "refresh_token_here",
  "token_type": "Bearer",
  "expires_in": 3600
}
```

**ProtonMail tokens (protonmail_oauth2_tokens.json):**
```json
{
  "access_token": "access_token_here",
  "refresh_token": "refresh_token_here",
  "token_type": "Bearer",
  "expires_in": 3600
}
```

**Zoho Mail tokens (zoho_mail_oauth2_tokens.json):**
```json
{
  "access_token": "access_token_here",
  "refresh_token": "refresh_token_here",
  "token_type": "Bearer",
  "expires_in": 3600
}
```

**Mailgun tokens (mailgun_oauth2_tokens.json):**
```json
{
  "access_token": "access_token_here",
  "refresh_token": "refresh_token_here",
  "token_type": "Bearer",
  "expires_in": 3600
}
```

**Fastmail tokens (fastmail_oauth2_tokens.json):**
```json
{
  "access_token": "access_token_here",
  "refresh_token": "refresh_token_here",
  "token_type": "Bearer",
  "expires_in": 3600
}
```

### ssmtp-mailer Integration

Add the tokens to your ssmtp-mailer configuration:

```ini
# /etc/ssmtp-mailer/auth/auth.conf

[Service_Accounts]
# Gmail OAuth2
google_oauth2_access_token = your_access_token_here
google_oauth2_refresh_token = your_refresh_token_here

# Office 365 OAuth2
microsoft_oauth2_access_token = your_access_token_here
microsoft_oauth2_refresh_token = your_refresh_token_here
```

## 🔒 Security Considerations

### Token Storage
- Tokens are saved locally in JSON files
- Keep these files secure and restrict access
- Consider using environment variables for production
- Rotate tokens regularly

### Network Security
- OAuth2 callbacks use localhost (127.0.0.1)
- No external network access required
- All communication is over HTTPS
- Tokens are transmitted securely

### Best Practices
- Use service accounts when possible
- Limit OAuth2 scopes to minimum required
- Monitor token usage and expiration
- Implement proper error handling

## 🐛 Troubleshooting

### Common Issues

**"Missing required dependency: requests"**
```bash
pip install requests
```

**"OAuth2 helper script not found"**
- Ensure you're running from the correct directory
- Check that the helper scripts exist
- Verify file permissions

**"Authentication failed"**
- Check your OAuth2 credentials
- Verify API permissions are enabled
- Ensure redirect URI matches exactly
- Check network connectivity

**"Port 8080 already in use"**
- The helper uses port 8080 for callbacks
- Close other applications using this port
- Or modify the port in the helper script

### Debug Mode

Enable verbose logging by setting environment variables:

```bash
# Enable debug output
export OAUTH2_DEBUG=1
export OAUTH2_VERBOSE=1

# Run helper with debug info
oauth2-helper gmail
```

## 📚 Additional Resources

### Documentation
- [Google OAuth2 Guide](https://developers.google.com/identity/protocols/oauth2)
- [Microsoft Graph Auth](https://docs.microsoft.com/en-us/graph/auth-overview)
- [ssmtp-mailer Configuration](docs/configuration/)

### Examples
- [Gmail OAuth2 Example](examples/gmail-oauth2-example.md)
- [Office 365 OAuth2 Example](examples/office365-oauth2-example.md)
- [Configuration Templates](examples/config-templates/)

### Support
- [GitHub Issues](https://github.com/blburns/ssmtp-mailer/issues)
- [Documentation](docs/)
- [Community Discussions](https://github.com/blburns/ssmtp-mailer/discussions)

## 🤝 Contributing

We welcome contributions to improve the OAuth2 helper tools!

### Adding New Providers
1. Create a new Python script in `python/`
2. Follow the existing pattern and interface
3. Add the provider to the launcher script
4. Update this README
5. Submit a pull request

### Improving Existing Helpers
- Bug fixes and improvements
- Better error handling
- Additional authentication methods
- Enhanced security features

## 📄 License

These OAuth2 helper tools are part of ssmtp-mailer and are licensed under the Apache License, Version 2.0.

---

**Need help?** Check the [ssmtp-mailer documentation](docs/) or [open an issue](https://github.com/blburns/ssmtp-mailer/issues) on GitHub.
