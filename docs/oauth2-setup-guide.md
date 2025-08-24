# OAuth2 Setup Guide for Gmail SMTP

## Overview

Since Google has phased out App Passwords for many accounts, you now need to use OAuth2 for Gmail SMTP authentication. This guide walks you through the process using our simple OAuth2 helper script.

## Prerequisites

- Python 3.6+ installed
- OAuth2 Client ID from Google Cloud Console
- OAuth2 Client Secret from Google Cloud Console
- Gmail account with 2FA enabled

## Step 1: Get OAuth2 Credentials

### 1.1 Google Cloud Console Setup

1. Go to [Google Cloud Console](https://console.cloud.google.com/)
2. Create a new project or select existing one
3. Enable the **Gmail API**:
   - **APIs & Services** → **Library**
   - Search for "Gmail API"
   - Click **Enable**

### 1.2 Create OAuth2 Credentials

1. **APIs & Services** → **Credentials**
2. Click **Create Credentials** → **OAuth 2.0 Client IDs**
3. Choose **Desktop application** as application type
4. Give it a name (e.g., "ssmtp-mailer")
5. Click **Create**
6. **Copy both the Client ID and Client Secret**

### 1.3 Configure OAuth Consent Screen

1. **APIs & Services** → **OAuth consent screen**
2. Choose **External** user type
3. Fill in required fields:
   - App name: "ssmtp-mailer"
   - User support email: your email
   - Developer contact information: your email
4. Add scopes: `https://mail.google.com/`
5. Add test users: your Gmail address
6. Save and continue

## Step 2: Use the OAuth2 Helper Script

### 2.1 Run the Helper Script

```bash
# Make the script executable
chmod +x scripts/oauth2-helper.py

# Run the script
python3 scripts/oauth2-helper.py
```

### 2.2 Follow the Interactive Process

1. **Enter your OAuth2 Client ID** when prompted
2. **Enter your OAuth2 Client Secret** when prompted
3. **Browser will open automatically** to Google's authorization page
4. **Sign in with your Gmail account**
5. **Grant permissions** to the application
6. **You'll be redirected back** to the helper script
7. **Tokens will be displayed** and saved to `oauth2_tokens.json`

## Step 3: Use the Refresh Token

### 3.1 Copy the Refresh Token

The script will display your tokens. **Copy the Refresh Token** - this is what you need for your configuration.

### 3.2 Update Your Domain Configuration

```ini
[domain:domain1.com]
enabled = true
smtp_server = smtp.gmail.com
smtp_port = 587
auth_method = OAUTH2
relay_account = mail-relay@domain1.com
username = mail-relay@domain1.com
oauth2_client_id = YOUR_CLIENT_ID_HERE
oauth2_client_secret = YOUR_CLIENT_SECRET_HERE
oauth2_refresh_token = YOUR_REFRESH_TOKEN_HERE
use_ssl = false
use_starttls = true
```

## Step 4: Test the OAuth2 Connection

### 4.1 Test with the Mailer Application

```bash
# Test OAuth2 authentication
ssmtp-mailer test --domain domain1.com

# Expected output:
# Testing SMTP connection for domain1.com...
# OAuth2 authentication successful!
# Connection test successful!
```

### 4.2 Send a Test Email

```bash
# Send test email using OAuth2
ssmtp-mailer send \
  --from contact-general@mailer.domain1.com \
  --to test@example.com \
  --subject "Test OAuth2 Email" \
  --body "This email was sent using OAuth2 authentication."
```

## How OAuth2 Works

### 1. **Authorization Flow**
```
User → Google Authorization → Authorization Code → Your App
```

### 2. **Token Exchange**
```
Authorization Code + Client Secret → Google → Access Token + Refresh Token
```

### 3. **SMTP Authentication**
```
Refresh Token → Google → New Access Token → SMTP Authentication
```

## Token Management

### Access Token
- **Expires in 1 hour**
- **Used for SMTP authentication**
- **Automatically refreshed** by the application

### Refresh Token
- **Long-lived** (can last months/years)
- **Used to get new access tokens**
- **Save this in your configuration**
- **Can expire** if not used for 6+ months

## Troubleshooting

### Common Issues

1. **"Invalid client" error**
   - Verify Client ID and Client Secret are correct
   - Check that credentials are for the right project

2. **"Redirect URI mismatch" error**
   - The helper script uses `http://localhost:8080`
   - Make sure this is added to your OAuth2 credentials

3. **"Access denied" error**
   - Check that your Gmail account is added as a test user
   - Verify the OAuth consent screen is configured

4. **"Scope not allowed" error**
   - Make sure `https://mail.google.com/` scope is added
   - Check that Gmail API is enabled

### Debug Commands

```bash
# Check OAuth2 configuration
ssmtp-mailer config --oauth2

# Test OAuth2 connection specifically
ssmtp-mailer test --oauth2 --domain domain1.com

# View OAuth2 tokens (if saved)
cat oauth2_tokens.json
```

## Security Considerations

1. **Keep Client Secret secure** - don't commit to version control
2. **Protect Refresh Token** - this gives long-term access
3. **Use environment variables** for sensitive credentials
4. **Rotate credentials** periodically

## Example Environment Variables

```bash
# In your shell or .env file
export OAUTH2_CLIENT_ID="your-client-id"
export OAUTH2_CLIENT_SECRET="your-client-secret"
export OAUTH2_REFRESH_TOKEN="your-refresh-token"
```

## Next Steps

1. **Test your OAuth2 setup** with a simple email
2. **Configure your mail relay system** with the new tokens
3. **Monitor logs** for any authentication issues
4. **Set up token refresh monitoring** if needed

## Support

If you encounter issues:

1. Check the OAuth2 helper script output
2. Verify Google Cloud Console settings
3. Test with a simple SMTP connection
4. Check application logs for detailed error messages

The OAuth2 flow is more complex than App Passwords, but it provides better security and is the future of Google authentication! 🚀
