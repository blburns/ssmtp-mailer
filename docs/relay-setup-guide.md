# Mail Relay Setup Guide

## Overview

This guide walks you through setting up the mail relay system for `dreamlikelabs.com` where emails sent to addresses on `mailer.dreamlikelabs.com` are automatically relayed to corresponding addresses on the main domain through Gmail SMTP.

## Prerequisites

- Domain: `dreamlikelabs.com`
- Subdomain: `mailer.dreamlikelabs.com`
- Gmail account: `mail-relay@dreamlikelabs.com`
- Server/VPS to run the mailer application
- DNS access to configure records

## Step 1: DNS Configuration

### 1.1 Main Domain Records

Add these records to your `dreamlikelabs.com` DNS:

```dns
# A record for your website
dreamlikelabs.com.          IN A      YOUR_WEBSITE_IP

# MX record for receiving emails
dreamlikelabs.com.          IN MX 10  mail.dreamlikelabs.com.

# SPF record for email authentication
dreamlikelabs.com.          IN TXT    "v=spf1 include:_spf.google.com ~all"

# DMARC record for email policy
_dmarc.dreamlikelabs.com.   IN TXT    "v=DMARC1; p=quarantine; rua=mailto:dmarc@dreamlikelabs.com"
```

### 1.2 Mailer Subdomain Records

Add these records for `mailer.dreamlikelabs.com`:

```dns
# A record pointing to your mailer server
mailer.dreamlikelabs.com.   IN A      YOUR_MAILER_SERVER_IP

# MX record for the mailer
mailer.dreamlikelabs.com.   IN MX 10  mailer.dreamlikelabs.com.

# SPF record for the mailer
mailer.dreamlikelabs.com.   IN TXT    "v=spf1 ip4:YOUR_MAILER_SERVER_IP ~all"
```

### 1.3 Reverse DNS (PTR)

Contact your hosting provider to set up reverse DNS:

```
YOUR_MAILER_SERVER_IP      IN PTR    mailer.dreamlikelabs.com.
```

## Step 2: Gmail Setup

### 2.1 Enable 2-Factor Authentication

1. Go to [Google Account Security](https://myaccount.google.com/security)
2. Enable 2-Step Verification
3. This is required for App Passwords

### 2.2 Generate App Password

1. Go to [App Passwords](https://myaccount.google.com/apppasswords)
2. Select "Mail" or "Other (Custom name)"
3. Enter "ssmtp-mailer" as the name
4. Copy the generated 16-character password
5. Save this password securely

## Step 3: Install Mailer Application

### 3.1 Build from Source

```bash
# Clone the repository
git clone https://github.com/your-repo/ssmtp-mailer.git
cd ssmtp-mailer

# Build the application
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install
sudo make install
```

### 3.2 Create Configuration Directory

```bash
# Create configuration directory
sudo mkdir -p /etc/ssmtp-mailer/{domains,users,mappings}

# Set permissions
sudo chown -R $USER:$USER /etc/ssmtp-mailer
```

## Step 4: Configuration Files

### 4.1 Main Configuration

Create `/etc/ssmtp-mailer/ssmtp-mailer.conf`:

```ini
[global]
default_hostname = mailer.dreamlikelabs.com
default_from = noreply@mailer.dreamlikelabs.com
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
```

### 4.2 Domain Configuration

Create `/etc/ssmtp-mailer/domains/dreamlikelabs.com.conf`:

```ini
[domain:dreamlikelabs.com]
enabled = true
smtp_server = smtp.gmail.com
smtp_port = 587
auth_method = LOGIN
relay_account = mail-relay@dreamlikelabs.com
username = mail-relay@dreamlikelabs.com
password = YOUR_GMAIL_APP_PASSWORD_HERE
use_ssl = false
use_starttls = true
```

Create `/etc/ssmtp-mailer/domains/mailer.dreamlikelabs.com.conf`:

```ini
[domain:mailer.dreamlikelabs.com]
enabled = true
smtp_server = localhost
smtp_port = 25
auth_method = NONE
relay_account = mail-relay@mailer.dreamlikelabs.com
use_ssl = false
use_starttls = false
```

### 4.3 User Configuration

Create `/etc/ssmtp-mailer/users/mail-relay@dreamlikelabs.com.conf`:

```ini
[user:mail-relay@dreamlikelabs.com]
enabled = true
domain = dreamlikelabs.com
can_send_from = true
can_send_to = true
template_address = false
allowed_recipients = *@dreamlikelabs.com
allowed_domains = dreamlikelabs.com
```

### 4.4 Address Mappings

Create `/etc/ssmtp-mailer/mappings/relay-mappings.conf`:

```ini
[mapping:contact-general]
from_pattern = contact-general@mailer.dreamlikelabs.com
to_pattern = contact-general@dreamlikelabs.com
smtp_account = mail-relay@dreamlikelabs.com
domain = dreamlikelabs.com

[mapping:contact-legal]
from_pattern = contact-legal@mailer.dreamlikelabs.com
to_pattern = contact-legal@dreamlikelabs.com
smtp_account = mail-relay@dreamlikelabs.com
domain = dreamlikelabs.com

[mapping:contact-privacy]
from_pattern = contact-privacy@mailer.dreamlikelabs.com
to_pattern = contact-privacy@dreamlikelabs.com
smtp_account = mail-relay@dreamlikelabs.com
domain = dreamlikelabs.com

[mapping:contact-support]
from_pattern = contact-support@mailer.dreamlikelabs.com
to_pattern = contact-support@dreamlikelabs.com
smtp_account = mail-relay@dreamlikelabs.com
domain = dreamlikelabs.com

[mapping:contact-sales]
from_pattern = contact-sales@mailer.dreamlikelabs.com
to_pattern = contact-sales@dreamlikelabs.com
smtp_account = mail-relay@dreamlikelabs.com
domain = dreamlikelabs.com
```

## Step 5: Testing

### 5.1 Test Configuration

```bash
# Test the configuration
ssmtp-mailer config

# Expected output:
# Configuration Status:
#   Mailer configured: Yes
#   Status: Ready
```

### 5.2 Test SMTP Connection

```bash
# Test connection to Gmail
ssmtp-mailer test

# Expected output:
# Testing SMTP connection...
# Connection test successful!
```

### 5.3 Test Email Relay

```bash
# Send a test email
ssmtp-mailer send \
  --from contact-general@mailer.dreamlikelabs.com \
  --to test@example.com \
  --subject "Test Email from Relay" \
  --body "This is a test email sent through the relay system."

# Expected output:
# Email sent successfully!
# Message ID: <generated-message-id>
```

## Step 6: Production Deployment

### 6.1 Systemd Service

Create `/etc/systemd/system/ssmtp-mailer.service`:

```ini
[Unit]
Description=ssmtp-mailer Relay Service
After=network.target

[Service]
Type=simple
User=ssmtp-mailer
Group=ssmtp-mailer
ExecStart=/usr/local/bin/ssmtp-mailer
Restart=always
RestartSec=10

[Install]
WantedBy=multi-user.target
```

### 6.2 Enable Service

```bash
# Create user
sudo useradd -r -s /bin/false ssmtp-mailer

# Set ownership
sudo chown -R ssmtp-mailer:ssmtp-mailer /etc/ssmtp-mailer

# Enable and start service
sudo systemctl enable ssmtp-mailer
sudo systemctl start ssmtp-mailer

# Check status
sudo systemctl status ssmtp-mailer
```

### 6.3 Log Rotation

Create `/etc/logrotate.d/ssmtp-mailer`:

```
/var/log/ssmtp-mailer.log {
    daily
    missingok
    rotate 52
    compress
    delaycompress
    notifempty
    create 644 ssmtp-mailer ssmtp-mailer
    postrotate
        systemctl reload ssmtp-mailer
    endscript
}
```

## Step 7: Monitoring

### 7.1 Check Logs

```bash
# View real-time logs
sudo tail -f /var/log/ssmtp-mailer.log

# Check for errors
sudo grep ERROR /var/log/ssmtp-mailer.log
```

### 7.2 Monitor Service

```bash
# Check service status
sudo systemctl status ssmtp-mailer

# Check service logs
sudo journalctl -u ssmtp-mailer -f
```

## Troubleshooting

### Common Issues

1. **DNS Not Propagated**
   - Wait 24-48 hours for DNS propagation
   - Test with different DNS servers
   - Verify records are correct

2. **Gmail Authentication Failed**
   - Verify App Password is correct
   - Ensure 2FA is enabled
   - Check account is not locked

3. **Connection Refused**
   - Check firewall settings
   - Verify server is listening on port 25
   - Check network connectivity

4. **Email Not Delivered**
   - Check SPF/DKIM/DMARC records
   - Verify reverse DNS
   - Check Gmail sending limits

### Debug Commands

```bash
# Test DNS resolution
nslookup mailer.dreamlikelabs.com
nslookup smtp.gmail.com

# Test SMTP manually
telnet smtp.gmail.com 587

# Check configuration
ssmtp-mailer config

# Test connection
ssmtp-mailer test
```

## Security Considerations

1. **Firewall Configuration**
   - Only allow necessary ports (25, 587, 465)
   - Restrict access to trusted IPs

2. **SSL/TLS**
   - Always use STARTTLS for Gmail
   - Verify SSL certificates

3. **Authentication**
   - Use App Passwords, not regular passwords
   - Rotate credentials regularly

4. **Rate Limiting**
   - Enable rate limiting in configuration
   - Monitor for abuse

## Maintenance

### Regular Tasks

1. **Update Credentials**
   - Rotate Gmail App Passwords quarterly
   - Update SSL certificates

2. **Monitor Logs**
   - Check for errors daily
   - Monitor disk space

3. **Backup Configuration**
   - Backup configuration files monthly
   - Test restore procedures

### Updates

1. **Application Updates**
   - Keep mailer application updated
   - Test updates in staging

2. **System Updates**
   - Apply security patches
   - Test after major updates

## Support

For issues:

1. Check logs: `/var/log/ssmtp-mailer.log`
2. Verify DNS configuration
3. Test SMTP connections manually
4. Check Gmail account status
5. Review firewall and network settings
