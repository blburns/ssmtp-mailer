# Quick Start Guide

Get ssmtp-mailer up and running in under 10 minutes! This guide covers the fastest path to a working email relay system.

## ⚡ Super Quick Start (5 minutes)

### 1. Install ssmtp-mailer
```bash
# Linux (Ubuntu/Debian)
curl -L -o ssmtp-mailer.deb "https://github.com/blburns/ssmtp-mailer/releases/latest/download/ssmtp-mailer_amd64.deb"
sudo dpkg -i ssmtp-mailer.deb

# macOS
curl -L -o ssmtp-mailer.dmg "https://github.com/blburns/ssmtp-mailer/releases/latest/download/ssmtp-mailer_macos.dmg"
hdiutil attach ssmtp-mailer.dmg
sudo installer -pkg /Volumes/ssmtp-mailer/ssmtp-mailer.pkg -target /
```

### 2. Set up OAuth2 (Gmail)
```bash
# Install Python helper
pip3 install requests

# Generate OAuth2 tokens
python3 tools/oauth2-helper/oauth2-helper.py gmail
# Follow the prompts and complete OAuth2 flow
```

### 3. Test Email Sending
```bash
# Test with sample email
ssmtp-mailer send \
  --from "your-email@gmail.com" \
  --to "test@example.com" \
  --subject "Test from ssmtp-mailer" \
  --body "Hello from ssmtp-mailer!"
```

## 🚀 Standard Quick Start (10 minutes)

### Prerequisites Check
```bash
# Check Python version
python3 --version  # Should be 3.6+

# Check if ssmtp-mailer is installed
ssmtp-mailer --version

# Check available OAuth2 providers
python3 tools/oauth2-helper/oauth2-helper.py --list
```

### Step 1: Choose Your Email Provider

#### Option A: Gmail (Most Popular)
```bash
# Set up Gmail OAuth2
python3 tools/oauth2-helper/oauth2-helper.py gmail
```

#### Option B: Office 365
```bash
# Set up Office 365 OAuth2
python3 tools/oauth2-helper/oauth2-helper.py office365
```

#### Option C: SendGrid
```bash
# Set up SendGrid OAuth2
python3 tools/oauth2-helper/oauth2-helper.py sendgrid
```

### Step 2: Configure ssmtp-mailer

#### Create Configuration File
```bash
# Create config directory
sudo mkdir -p /etc/ssmtp-mailer
sudo chown $USER:$USER /etc/ssmtp-mailer

# Create configuration file
cat > /etc/ssmtp-mailer/config.json << 'EOF'
{
  "smtp": {
    "host": "smtp.gmail.com",
    "port": 587,
    "security": "tls",
    "auth": {
      "type": "oauth2",
      "client_id": "YOUR_CLIENT_ID",
      "client_secret": "YOUR_CLIENT_SECRET",
      "refresh_token": "YOUR_REFRESH_TOKEN",
      "user": "your-email@gmail.com"
    }
  }
}
EOF
```

#### Update with Your Tokens
```bash
# Replace placeholders with actual values from OAuth2 helper
# Edit the config.json file with your credentials
nano /etc/ssmtp-mailer/config.json
```

### Step 3: Test Your Setup

#### Basic Test
```bash
# Test configuration
ssmtp-mailer test --config /etc/ssmtp-mailer/config.json

# Send test email
ssmtp-mailer send \
  --config /etc/ssmtp-mailer/config.json \
  --from "your-email@gmail.com" \
  --to "your-email@gmail.com" \
  --subject "ssmtp-mailer Test" \
  --body "If you receive this, ssmtp-mailer is working!"
```

#### Advanced Test
```bash
# Test with HTML content
ssmtp-mailer send \
  --config /etc/ssmtp-mailer/config.json \
  --from "your-email@gmail.com" \
  --to "test@example.com" \
  --subject "HTML Test" \
  --body "<h1>Hello World</h1><p>This is an HTML email!</p>" \
  --html

# Test with attachments
ssmtp-mailer send \
  --config /etc/ssmtp-mailer/config.json \
  --from "your-email@gmail.com" \
  --to "test@example.com" \
  --subject "Attachment Test" \
  --body "See attached file" \
  --attach /path/to/file.txt
```

## 🔧 Configuration Options

### Basic Configuration
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

### Advanced Configuration
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
  },
  "logging": {
    "level": "info",
    "file": "/var/log/ssmtp-mailer/ssmtp-mailer.log"
  },
  "queue": {
    "enabled": true,
    "max_retries": 3,
    "retry_delay": 300
  }
}
```

## 🧪 Testing Commands

### Configuration Testing
```bash
# Test configuration syntax
ssmtp-mailer test --config /etc/ssmtp-mailer/config.json

# Test SMTP connection
ssmtp-mailer test --config /etc/ssmtp-mailer/config.json --smtp

# Test OAuth2 authentication
ssmtp-mailer test --config /etc/ssmtp-mailer/config.json --auth
```

### Email Testing
```bash
# Simple text email
ssmtp-mailer send \
  --config /etc/ssmtp-mailer/config.json \
  --from "your-email@gmail.com" \
  --to "test@example.com" \
  --subject "Test" \
  --body "Hello World"

# HTML email
ssmtp-mailer send \
  --config /etc/ssmtp-mailer/config.json \
  --from "your-email@gmail.com" \
  --to "test@example.com" \
  --subject "HTML Test" \
  --body "<h1>Hello</h1>" \
  --html

# Email with attachment
ssmtp-mailer send \
  --config /etc/ssmtp-mailer/config.json \
  --from "your-email@gmail.com" \
  --to "test@example.com" \
  --subject "File Test" \
  --body "See attached" \
  --attach /path/to/file.txt
```

## 🚨 Common Issues & Quick Fixes

### "Command not found: ssmtp-mailer"
```bash
# Add to PATH or use full path
export PATH="/usr/local/bin:$PATH"
# or
/usr/local/bin/ssmtp-mailer --version
```

### "OAuth2 authentication failed"
```bash
# Check token file exists
ls -la oauth2_tokens.json

# Regenerate tokens
python3 tools/oauth2-helper/oauth2-helper.py gmail
```

### "SMTP connection failed"
```bash
# Check network connectivity
telnet smtp.gmail.com 587

# Verify firewall settings
sudo ufw status
```

### "Permission denied"
```bash
# Fix file permissions
sudo chown $USER:$USER /etc/ssmtp-mailer/config.json
chmod 600 /etc/ssmtp-mailer/config.json
```

## 📚 Next Steps

### Basic Usage
- **Send emails** using the command line interface
- **Configure logging** for monitoring and debugging
- **Set up email templates** for consistent messaging

### Advanced Features
- **Email queuing** for high-volume sending
- **Rate limiting** to avoid provider restrictions
- **Monitoring and alerting** for production use

### Production Deployment
- **SSL/TLS configuration** for secure transmission
- **DNS setup** (SPF, DKIM, DMARC) for deliverability
- **Monitoring and maintenance** procedures

## 🆘 Need Help?

### Quick Troubleshooting
1. **Check logs**: `tail -f /var/log/ssmtp-mailer/ssmtp-mailer.log`
2. **Verify config**: `ssmtp-mailer test --config /etc/ssmtp-mailer/config.json`
3. **Test OAuth2**: Regenerate tokens if needed
4. **Check network**: Verify connectivity to SMTP servers

### Documentation
- **[Installation Guides](../installation/README.md)** - Platform-specific setup
- **[OAuth2 Setup](../oauth2/README.md)** - Authentication configuration
- **[Configuration Guide](../configuration/README.md)** - Advanced settings
- **[Troubleshooting](../oauth2/oauth2-troubleshooting.md)** - Common issues

### Community Support
- **GitHub Issues**: Report bugs and request features
- **GitHub Discussions**: Ask questions and share solutions

---

*You're all set! ssmtp-mailer is now running and ready to send emails. For advanced configuration and production deployment, check out the [full documentation](../README.md).*
