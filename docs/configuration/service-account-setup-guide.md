# Service Account Setup Guide for Gmail SMTP

## Overview

This guide provides detailed, step-by-step instructions for setting up Google Service Account authentication for your mail relay system. Service Account authentication is more secure than App Passwords and provides fully automated token refresh.

## Prerequisites

- **Google Cloud Project** with billing enabled
- **Google Workspace Admin access** (for domain-wide delegation)
- **Domain ownership** verified in Google Cloud Console
- **Gmail API enabled** in your project
- **Ubuntu/Linux server** for running the mail relay

## Step 1: Google Cloud Console Setup

### 1.1 Access Google Cloud Console

1. **Go to [Google Cloud Console](https://console.cloud.google.com/)**
2. **Sign in** with your Google account
3. **Select or create a project**:
   - If you have an existing project, select it from the dropdown
   - If you need a new project, click **"New Project"**
   - Enter a **Project name** (e.g., "mail-relay-system")
   - Click **"Create"**

### 1.2 Enable Required APIs

1. **Navigate to APIs & Services** → **Library**
2. **Search for and enable these APIs**:
   - **Gmail API** - Required for SMTP access
   - **Google+ API** - Required for domain-wide delegation
   - **Admin SDK** - Required for domain management

3. **For each API**:
   - Click on the API name
   - Click **"Enable"**
   - Wait for the API to be enabled

### 1.3 Verify Domain Ownership

1. **Go to APIs & Services** → **Credentials**
2. **Click "Configure Consent Screen"**
3. **Choose user type**:
   - Select **"External"** (unless you have Google Workspace)
   - Click **"Create"**

4. **Fill in required information**:
   - **App name**: Your mail relay app name (e.g., "Mail Relay System")
   - **User support email**: Your email address
   - **Developer contact information**: Your email address
   - Click **"Save and Continue"**

5. **Add scopes**:
   - Click **"Add or Remove Scopes"**
   - Search for and select: `https://mail.google.com/`
   - Click **"Update"**
   - Click **"Save and Continue"**

6. **Add test users**:
   - Click **"Add Users"**
   - Add your email address and any other test users
   - Click **"Save and Continue"**

7. **Review and publish**:
   - Review your settings
   - Click **"Back to Dashboard"**

## Step 2: Create Service Account

### 2.1 Create the Service Account

1. **Go to APIs & Services** → **Credentials**
2. **Click "Create Credentials"** → **"Service Account"**
3. **Fill in service account details**:
   - **Service account name**: `mail-relay-service` (or your preferred name)
   - **Service account ID**: Auto-generated (you can customize)
   - **Description**: `Service account for mail relay system authentication`
   - Click **"Create and Continue"**

4. **Grant access** (optional for now):
   - Click **"Continue"** (skip role assignment)
   - Click **"Done"**

### 2.2 Enable Domain-Wide Delegation

1. **In the Credentials list**, click on your newly created service account
2. **Go to the "Keys" tab**
3. **Click "Add Key"** → **"Create new key"**
4. **Choose key type**:
   - Select **"JSON"**
   - Click **"Create"**
5. **Download the JSON file**:
   - The file will automatically download
   - **Keep this file secure** - it contains your private key
   - Note the **Client ID** shown in the service account details

### 2.3 Service Account Details

**Important information to note:**
- **Service Account Email**: `mail-relay-service@your-project-id.iam.gserviceaccount.com`
- **Client ID**: A long numeric string (you'll need this for domain-wide delegation)
- **Project ID**: Your Google Cloud project ID
- **Private Key**: Embedded in the downloaded JSON file

## Step 3: Google Workspace Admin Configuration

### 3.1 Access Google Workspace Admin

1. **Go to [Google Admin Console](https://admin.google.com/)**
2. **Sign in** with your Google Workspace admin account
3. **Navigate to Security** → **Access and data control** → **API controls**

### 3.2 Configure Domain-Wide Delegation

1. **Click "Manage Domain Wide Delegation"**
2. **Click "Add new"**
3. **Fill in the delegation details**:
   - **Client ID**: Paste your service account Client ID from Step 2.2
   - **OAuth Scopes**: `https://mail.google.com/`
4. **Click "Authorize"**

**Important Notes:**
- The **Client ID** must match exactly what's in your service account
- The **OAuth scope** must be exactly `https://mail.google.com/`
- This step is **critical** - without it, the service account cannot act on behalf of users

### 3.3 Verify Configuration

1. **Check that the delegation appears** in the list
2. **Verify the Client ID matches** your service account
3. **Confirm the scope is correct**

## Step 4: Automated Setup Script

### 4.1 Run the Setup Script

```bash
# Make sure you're in the project directory
cd ~/ssmtp-mailer

# Make the script executable
chmod +x scripts/setup-service-account.sh

# Run the setup script
./scripts/setup-service-account.sh
```

### 4.2 Script Interactive Process

The script will guide you through:

1. **Dependency checking** - Ensures required tools are installed
2. **Directory creation** - Sets up configuration structure
3. **Domain information** - Collects your domain details
4. **Google Cloud setup** - Guides you through the steps above
5. **Configuration generation** - Creates all necessary config files
6. **File permissions** - Sets secure permissions for sensitive files

### 4.3 What the Script Creates

```
/etc/ssmtp-mailer/
├── ssmtp-mailer.conf              # Main configuration
├── service-accounts/               # Service account JSON files
│   └── domain1-service-account.json
├── domains/                        # Domain configurations
│   ├── domain1.com.conf
│   └── mailer.domain1.com.conf
├── users/                          # User configurations
│   └── mail-relay@domain1.com
└── mappings/                       # Address mappings
    └── domain1-mappings.conf
```

## Step 5: Manual Configuration (Alternative)

If you prefer to configure manually instead of using the script:

### 5.1 Create Configuration Directory

```bash
sudo mkdir -p /etc/ssmtp-mailer/{service-accounts,domains,users,mappings}
sudo chown -R $USER:$USER /etc/ssmtp-mailer
sudo chmod 700 /etc/ssmtp-mailer/service-accounts
```

### 5.2 Place Service Account JSON

```bash
# Copy your downloaded JSON file
cp ~/Downloads/your-service-account.json /etc/ssmtp-mailer/service-accounts/domain1-service-account.json

# Set secure permissions
chmod 600 /etc/ssmtp-mailer/service-accounts/domain1-service-account.json
```

### 5.3 Create Domain Configuration

Create `/etc/ssmtp-mailer/domains/domain1.com.conf`:

```ini
[domain:domain1.com]
enabled = true
smtp_server = smtp.gmail.com
smtp_port = 587

# Service Account Authentication
auth_method = SERVICE_ACCOUNT
service_account_file = /etc/ssmtp-mailer/service-accounts/domain1-service-account.json
service_account_user = mail-relay@domain1.com

# Remote relay account (Gmail account being impersonated)
relay_account = mail-relay@domain1.com

# Gmail SSL/TLS settings
use_ssl = false
use_starttls = true
```

### 5.4 Create Main Configuration

Create `/etc/ssmtp-mailer/ssmtp-mailer.conf`:

```ini
[global]
default_hostname = mailer.domain1.com
default_from = noreply@mailer.domain1.com
config_dir = /etc/ssmtp-mailer
domains_dir = /etc/ssmtp-mailer/domains
users_dir = /etc/ssmtp-mailer/users
mappings_dir = /etc/ssmtp-mailer/mappings
log_file = /var/log/ssmtp-mailer.log
log_level = INFO
max_connections = 10
connection_timeout = 30
read_timeout = 60
write_timeout = 60
enable_rate_limiting = true
rate_limit_per_minute = 100

# Service Account Configuration
service_accounts_dir = /etc/ssmtp-mailer/service-accounts
```

## Step 6: Testing and Validation

### 6.1 Test Configuration

```bash
# Test the configuration
./bin/ssmtp-mailer config

# Test SMTP connections
./bin/ssmtp-mailer test

# Check service account authentication
./bin/ssmtp-mailer test --domain domain1.com
```

### 6.2 Send Test Email

```bash
# Send a test email
./bin/ssmtp-mailer send \
  --from contact-general@mailer.domain1.com \
  --to test@example.com \
  --subject "Service Account Test" \
  --body "This email was sent using Service Account authentication"
```

### 6.3 Monitor Logs

```bash
# Watch the logs in real-time
tail -f /var/log/ssmtp-mailer.log

# Check for authentication success
grep "Service Account" /var/log/ssmtp-mailer.log
grep "JWT" /var/log/ssmtp-mailer.log
```

## Step 7: Multiple Domain Setup

### 7.1 Additional Domains

For multiple domains, repeat the process:

1. **Create separate service accounts** for each domain
2. **Configure domain-wide delegation** for each service account
3. **Create separate configuration files** for each domain
4. **Use different service account JSON files**

### 7.2 Example Multi-Domain Structure

```
/etc/ssmtp-mailer/
├── service-accounts/
│   ├── domain1-service-account.json
│   ├── domain2-service-account.json
│   └── domain3-service-account.json
├── domains/
│   ├── domain1.com.conf
│   ├── domain2.com.conf
│   └── domain3.com.conf
└── mappings/
    ├── domain1-mappings.conf
    ├── domain2-mappings.conf
    └── domain3-mappings.conf
```

### 7.3 Domain-Specific Configurations

Each domain can have different settings:

```ini
# domain1.com.conf
[domain:domain1.com]
auth_method = SERVICE_ACCOUNT
service_account_file = /etc/ssmtp-mailer/service-accounts/domain1-service-account.json
service_account_user = mail-relay@domain1.com

# domain2.com.conf
[domain:domain2.com]
auth_method = SERVICE_ACCOUNT
service_account_file = /etc/ssmtp-mailer/service-accounts/domain2-service-account.json
service_account_user = mail-relay@domain2.com
```

## Troubleshooting

### Common Issues and Solutions

#### 1. "Service account not found" Error

**Symptoms:**
- Configuration file not found
- Service account JSON parsing errors

**Solutions:**
- Verify the JSON file path in your configuration
- Check file permissions (should be 600)
- Ensure the JSON file is valid JSON

#### 2. "Domain-wide delegation not configured" Error

**Symptoms:**
- Authentication fails with delegation errors
- Service account cannot impersonate users

**Solutions:**
- Verify Client ID matches exactly in Google Workspace Admin
- Check OAuth scope is exactly `https://mail.google.com/`
- Ensure you have admin access to Google Workspace

#### 3. "JWT token creation failed" Error

**Symptoms:**
- Private key parsing errors
- JWT signature generation fails

**Solutions:**
- Verify the private key in your JSON file
- Check that OpenSSL is properly installed
- Ensure the JSON file contains all required fields

#### 4. "Token exchange failed" Error

**Symptoms:**
- HTTP errors during token exchange
- Google API rejects the JWT

**Solutions:**
- Verify the service account email in JWT payload
- Check that Gmail API is enabled
- Ensure the JWT hasn't expired

### Debug Commands

```bash
# Check service account JSON validity
cat /etc/ssmtp-mailer/service-accounts/domain1-service-account.json | jq .

# Verify file permissions
ls -la /etc/ssmtp-mailer/service-accounts/

# Test JWT creation manually
./bin/ssmtp-mailer debug --service-account domain1.com

# Check Google Cloud Console logs
# Go to: APIs & Services > Credentials > Service Accounts > [Your Account] > Logs
```

### Log Analysis

**Successful authentication logs:**
```
[INFO] Service Account Auth initialized successfully for: mail-relay-service@project.iam.gserviceaccount.com
[DEBUG] Generated new access token, expires in 1 hour
[INFO] SMTP authentication successful using Service Account
```

**Error logs to watch for:**
```
[ERROR] Failed to load service account: [file path]
[ERROR] Domain-wide delegation not configured
[ERROR] JWT token creation failed
[ERROR] Token exchange failed with HTTP code: 400
```

## Security Best Practices

### 1. File Permissions

```bash
# Service account JSON files should be 600
chmod 600 /etc/ssmtp-mailer/service-accounts/*.json

# Configuration directory should be 700
chmod 700 /etc/ssmtp-mailer/service-accounts/

# Only root or mail user should access
sudo chown root:mail /etc/ssmtp-mailer/service-accounts/
```

### 2. Key Rotation

- **Rotate service account keys** every 90 days
- **Monitor key usage** in Google Cloud Console
- **Use different keys** for different environments

### 3. Access Control

- **Limit service account permissions** to only what's needed
- **Use IAM roles** to restrict access
- **Monitor service account usage** regularly

### 4. Network Security

- **Use HTTPS** for all API communications
- **Restrict access** to your mail relay server
- **Monitor network traffic** for unusual patterns

## Monitoring and Maintenance

### 1. Regular Health Checks

```bash
# Daily monitoring script
#!/bin/bash
./bin/ssmtp-mailer test --domain domain1.com
./bin/ssmtp-mailer test --domain domain2.com
./bin/ssmtp-mailer test --domain domain3.com
```

### 2. Log Rotation

```bash
# Configure logrotate for /var/log/ssmtp-mailer.log
/var/log/ssmtp-mailer.log {
    daily
    rotate 30
    compress
    delaycompress
    missingok
    notifempty
    create 644 root root
}
```

### 3. Performance Monitoring

- **Monitor token refresh frequency**
- **Track SMTP connection success rates**
- **Monitor email delivery statistics**

## Advanced Configuration

### 1. Custom JWT Claims

You can customize JWT tokens with additional claims:

```cpp
// In service_account_auth.cpp
std::string createJWTPayload() {
    // Add custom claims
    payload["custom_claim"] = "mail_relay_system";
    payload["environment"] = "production";
    // ... rest of payload
}
```

### 2. Token Caching

Implement token caching to reduce API calls:

```cpp
// Cache tokens in memory or Redis
std::map<std::string, CachedToken> token_cache_;
```

### 3. Load Balancing

For high-traffic scenarios:

```ini
# Multiple service accounts for load balancing
[domain:domain1.com]
auth_method = SERVICE_ACCOUNT_ROUND_ROBIN
service_account_files = /path/to/account1.json,/path/to/account2.json
```

## Support and Resources

### 1. Official Documentation

- [Google Cloud Service Accounts](https://cloud.google.com/iam/docs/service-accounts)
- [Gmail API Documentation](https://developers.google.com/gmail/api)
- [Domain-Wide Delegation](https://developers.google.com/admin-sdk/directory/v1/guides/delegation)

### 2. Community Resources

- [Google Cloud Community](https://cloud.google.com/community)
- [Stack Overflow](https://stackoverflow.com/questions/tagged/google-cloud-platform)
- [GitHub Issues](https://github.com/your-repo/issues)

### 3. Getting Help

When reporting issues, include:
- **Service account configuration** (with sensitive data removed)
- **Error logs** from the application
- **Google Cloud Console logs**
- **Steps to reproduce** the issue
- **Environment details** (OS, versions, etc.)

---

## Summary

Service Account authentication provides a secure, automated way to authenticate with Gmail SMTP. By following this guide, you'll have:

✅ **Secure authentication** using cryptographic keys  
✅ **Automatic token refresh** - no manual intervention  
✅ **Domain-wide delegation** for enterprise use  
✅ **Production-ready configuration** with proper security  
✅ **Comprehensive monitoring** and troubleshooting  

**Remember**: Keep your service account keys secure and rotate them regularly for maximum security! 🚀
