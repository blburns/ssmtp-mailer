# OAuth2 Helper Tools Guide

## Overview

ssmtp-mailer includes comprehensive OAuth2 helper tools that automate the process of setting up OAuth2 authentication for various email providers. These tools handle the entire OAuth2 flow, from authorization to token generation and storage.

## 🛠️ Available Tools

### Command Line Launcher
The main launcher script that provides access to all OAuth2 providers:

```bash
python3 tools/oauth2-helper/oauth2-helper.py [provider]
```

### Individual Provider Scripts
Each provider has its own dedicated Python script:

```bash
# Gmail
python3 tools/oauth2-helper/python/gmail-oauth2-helper.py

# Office 365
python3 tools/oauth2-helper/python/office365-oauth2-helper.py

# SendGrid
python3 tools/oauth2-helper/python/sendgrid-oauth2-helper.py

# Amazon SES
python3 tools/oauth2-helper/python/amazon-ses-oauth2-helper.py

# ProtonMail
python3 tools/oauth2-helper/python/protonmail-oauth2-helper.py

# Zoho Mail
python3 tools/oauth2-helper/python/zoho-mail-oauth2-helper.py

# Mailgun
python3 tools/oauth2-helper/python/mailgun-oauth2-helper.py

# Fastmail
python3 tools/oauth2-helper/python/fastmail-oauth2-helper.py
```

### Web-Based Tools
HTML interfaces for non-technical users:

```bash
# Gmail web helper
open tools/oauth2-helper/web/gmail-oauth2-helper.html

# Serve locally for testing
python3 -m http.server 8000
# Then open http://localhost:8000/tools/oauth2-helper/web/gmail-oauth2-helper.html
```

## 🚀 Getting Started

### 1. Prerequisites
```bash
# Install Python dependencies
pip3 install requests

# Or install from requirements file
pip3 install -r tools/oauth2-helper/requirements.txt
```

### 2. List Available Providers
```bash
python3 tools/oauth2-helper/oauth2-helper.py --list
```

### 3. Launch Provider Helper
```bash
# Using the launcher
python3 tools/oauth2-helper/oauth2-helper.py gmail

# Or directly
python3 tools/oauth2-helper/python/gmail-oauth2-helper.py
```

## 📱 Using the Helper Tools

### Step-by-Step Process

#### 1. Launch the Helper
```bash
python3 tools/oauth2-helper/oauth2-helper.py gmail
```

#### 2. Enter Credentials
The helper will prompt you for:
- **OAuth2 Client ID** (from your provider's developer console)
- **OAuth2 Client Secret** (from your provider's developer console)
- **Additional provider-specific fields** (e.g., Tenant ID for Office 365)

#### 3. Authorization Flow
1. **Helper generates** authorization URL
2. **Copy the URL** to your browser
3. **Sign in** with your email account
4. **Grant permissions** to the application
5. **Return to terminal** when complete

#### 4. Token Generation
1. **Helper starts** local callback server
2. **Provider redirects** back with authorization code
3. **Helper exchanges** code for tokens
4. **Tokens are saved** to JSON file

#### 5. Success!
- **Access token** and **refresh token** are displayed
- **Token file** is saved locally
- **Configuration instructions** are provided

## 🔧 Tool Features

### Automatic Token Management
- **Local callback server** for OAuth2 flow
- **Automatic token exchange** from authorization code
- **Secure token storage** in JSON format
- **Error handling** and user feedback

### Cross-Platform Support
- **Python-based** for maximum compatibility
- **Local HTTP server** for OAuth2 callbacks
- **Browser integration** for user authentication
- **File-based output** for easy integration

### Security Features
- **Local callback server** (no external dependencies)
- **Secure token storage** in local files
- **No credential logging** in terminal output
- **State parameter validation** for CSRF protection

## 📁 Output Files

### Token Files
Each provider generates a specific token file:

| Provider | Token File |
|----------|------------|
| Gmail | `oauth2_tokens.json` |
| Office 365 | `office365_oauth2_tokens.json` |
| SendGrid | `sendgrid_oauth2_tokens.json` |
| Amazon SES | `amazon_ses_oauth2_tokens.json` |
| ProtonMail | `protonmail_oauth2_tokens.json` |
| Zoho Mail | `zoho_mail_oauth2_tokens.json` |
| Mailgun | `mailgun_oauth2_tokens.json` |
| Fastmail | `fastmail_oauth2_tokens.json` |

### Token File Format
```json
{
  "access_token": "ya29.a0AfH6SMB...",
  "refresh_token": "1//04d...",
  "token_type": "Bearer",
  "expires_in": 3599,
  "scope": "https://mail.google.com/"
}
```

## 🔒 Security Considerations

### Local Development
- **Use localhost** for testing and development
- **Keep token files** in secure locations
- **Don't commit** token files to version control
- **Use environment variables** for production

### Production Use
- **HTTPS redirect URIs** required for production
- **Secure token storage** in production systems
- **Regular credential rotation** recommended
- **Monitor authentication** logs

### Best Practices
- **Store tokens securely** (file permissions 600)
- **Use refresh tokens** for long-term access
- **Monitor token expiration** and refresh
- **Implement error handling** for token failures

## 🧪 Testing and Validation

### Verify Token Validity
```bash
# Test Gmail token
curl -H "Authorization: Bearer YOUR_ACCESS_TOKEN" \
     "https://gmail.googleapis.com/gmail/v1/users/me/profile"

# Test Office 365 token
curl -H "Authorization: Bearer YOUR_ACCESS_TOKEN" \
     "https://graph.microsoft.com/v1.0/me"
```

### Test SMTP Connection
```bash
# Test with your mailer application
ssmtp-mailer test

# Or test manually with openssl
openssl s_client -connect smtp.gmail.com:587 -starttls smtp
```

## 🆘 Troubleshooting

### Common Issues

#### "Address already in use" Error
```bash
# Port 8080 is already in use
# Solution: Kill existing process or use different port
lsof -ti:8080 | xargs kill -9
```

#### "Module not found" Error
```bash
# Missing Python dependencies
pip3 install requests
```

#### "Invalid redirect URI" Error
- **Cause**: Redirect URI doesn't match provider configuration
- **Fix**: Update redirect URIs in your provider's developer console

#### "Authorization failed" Error
- **Cause**: User denied permissions or consent screen issues
- **Fix**: Check provider's OAuth2 consent screen configuration

### Debug Mode
```bash
# Run with verbose output
python3 tools/oauth2-helper/oauth2-helper.py gmail --verbose

# Check browser console for web-based tools
# Check terminal output for Python errors
```

## 📚 Advanced Usage

### Custom Redirect URIs
```bash
# Set custom redirect URI (if supported by provider)
export OAUTH2_REDIRECT_URI="http://localhost:9000"
python3 tools/oauth2-helper/oauth2-helper.py gmail
```

### Batch Token Generation
```bash
# Generate tokens for multiple providers
for provider in gmail office365 sendgrid; do
    echo "Setting up $provider..."
    python3 tools/oauth2-helper/oauth2-helper.py $provider
done
```

### Integration with CI/CD
```bash
# Use in automated environments
python3 tools/oauth2-helper/oauth2-helper.py gmail \
    --client-id "$GMAIL_CLIENT_ID" \
    --client-secret "$GMAIL_CLIENT_SECRET" \
    --non-interactive
```

## 🔗 Integration with ssmtp-mailer

### Configuration Files
Use the generated tokens in your ssmtp-mailer configuration:

```json
{
  "smtp": {
    "host": "smtp.gmail.com",
    "port": 587,
    "security": "tls",
    "auth": {
      "type": "oauth2",
      "client_id": "your-client-id",
      "client_secret": "your-client-secret",
      "refresh_token": "your-refresh-token",
      "user": "your-email@gmail.com"
    }
  }
}
```

### Environment Variables
```bash
# Set environment variables for production
export GMAIL_CLIENT_ID="your-client-id"
export GMAIL_CLIENT_SECRET="your-client-secret"
export GMAIL_REFRESH_TOKEN="your-refresh-token"
```

## 📖 Provider-Specific Guides

For detailed setup instructions for each provider:

- **[Gmail OAuth2 Setup](gmail-oauth2-setup.md)**
- **[Office 365 OAuth2 Setup](office365-oauth2-setup.md)**
- **[SendGrid OAuth2 Setup](sendgrid-oauth2-setup.md)**
- **[Amazon SES OAuth2 Setup](amazon-ses-oauth2-setup.md)**
- **[ProtonMail OAuth2 Setup](protonmail-oauth2-setup.md)**
- **[Zoho Mail OAuth2 Setup](zoho-mail-oauth2-setup.md)**
- **[Mailgun OAuth2 Setup](mailgun-oauth2-setup.md)**
- **[Fastmail OAuth2 Setup](fastmail-oauth2-setup.md)**

## 🔄 Token Refresh

### Automatic Refresh
ssmtp-mailer automatically handles token refresh using the refresh token:

1. **Access token expires** (usually 1 hour)
2. **ssmtp-mailer detects** expiration
3. **Uses refresh token** to get new access token
4. **Continues operation** seamlessly

### Manual Refresh
```bash
# If you need to refresh tokens manually
python3 tools/oauth2-helper/oauth2-helper.py gmail --refresh
```

## 📋 Summary

The OAuth2 helper tools provide a complete solution for setting up OAuth2 authentication with various email providers:

- ✅ **Automated OAuth2 flow** handling
- ✅ **Multiple provider support** (8 providers)
- ✅ **Cross-platform compatibility**
- ✅ **Secure token management**
- ✅ **Easy integration** with ssmtp-mailer
- ✅ **Comprehensive error handling**

**Start with**: `python3 tools/oauth2-helper/oauth2-helper.py --list`

**Choose your provider**: `python3 tools/oauth2-helper/oauth2-helper.py [provider]`

**Follow the prompts** and you'll have OAuth2 tokens ready for use! 🚀

---

*For specific provider setup instructions, see the individual provider guides listed above.*
