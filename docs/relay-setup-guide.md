# Mail Relay Setup Guide

## Overview

This guide walks you through setting up the mail relay system for multiple domains (`domain1.com`, `domain2.com`, `domain3.com`) where emails sent to addresses on `mailer.domain1.com`, `mailer.domain2.com`, and `mailer.domain3.com` are automatically relayed to corresponding addresses on the main domains through Gmail SMTP.

## Prerequisites

- Domains: `domain1.com`, `domain2.com`, `domain3.com`
- Subdomains: `mailer.domain1.com`, `mailer.domain2.com`, `mailer.domain3.com`
- Gmail accounts: `mail-relay@domain1.com`, `mail-relay@domain2.com`, `mail-relay@domain3.com`
- Server/VPS to run the mailer application
- DNS access to configure records for all domains

## Step 1: DNS Configuration

### 1.1 Main Domain Records

Add these records to each of your domains:

#### domain1.com DNS Records

```dns
# A record for your website
domain1.com.          IN A      YOUR_WEBSITE_IP

# MX record for receiving emails
domain1.com.          IN MX 10  mail.domain1.com.

# SPF record for email authentication
domain1.com.          IN TXT    "v=spf1 include:_spf.google.com ~all"

# DMARC record for email policy
_dmarc.domain1.com.   IN TXT    "v=DMARC1; p=quarantine; rua=mailto:dmarc@domain1.com"
```

#### domain2.com DNS Records

```dns
# A record for your website
domain2.com.          IN A      YOUR_WEBSITE_IP

# MX record for receiving emails
domain2.com.          IN MX 10  mail.domain2.com.

# SPF record for email authentication
domain2.com.          IN TXT    "v=spf1 include:_spf.google.com ~all"

# DMARC record for email policy
_dmarc.domain2.com.   IN TXT    "v=DMARC1; p=quarantine; rua=mailto:dmarc@domain2.com"
```

#### domain3.com DNS Records

```dns
# A record for your website
domain3.com.          IN A      YOUR_WEBSITE_IP

# MX record for receiving emails
domain3.com.          IN MX 10  mail.domain3.com.

# SPF record for email authentication
domain3.com.          IN TXT    "v=spf1 include:_spf.google.com ~all"

# DMARC record for email policy
_dmarc.domain3.com.   IN TXT    "v=DMARC1; p=quarantine; rua=mailto:dmarc@domain3.com"
```

### 1.2 Mailer Subdomain Records

Add these records for each mailer subdomain:

#### mailer.domain1.com DNS Records

```dns
# A record pointing to your mailer server
mailer.domain1.com.   IN A      YOUR_MAILER_SERVER_IP

# MX record for the mailer
mailer.domain1.com.   IN MX 10  mailer.domain1.com.

# SPF record for the mailer
mailer.domain1.com.   IN TXT    "v=spf1 ip4:YOUR_MAILER_SERVER_IP ~all"
```

#### mailer.domain2.com DNS Records

```dns
# A record pointing to your mailer server
mailer.domain2.com.   IN A      YOUR_MAILER_SERVER_IP

# MX record for the mailer
mailer.domain2.com.   IN MX 10  mailer.domain2.com.

# SPF record for the mailer
mailer.domain2.com.   IN TXT    "v=spf1 ip4:YOUR_MAILER_SERVER_IP ~all"
```

#### mailer.domain3.com DNS Records

```dns
# A record pointing to your mailer server
mailer.domain3.com.   IN A      YOUR_MAILER_SERVER_IP

# MX record for the mailer
mailer.domain3.com.   IN MX 10  mailer.domain3.com.

# SPF record for the mailer
mailer.domain3.com.   IN TXT    "v=spf1 ip4:YOUR_MAILER_SERVER_IP ~all"
```

### 1.3 Reverse DNS (PTR)

Contact your hosting provider to set up reverse DNS for all mailer subdomains:

```
YOUR_MAILER_SERVER_IP      IN PTR    mailer.domain1.com.
YOUR_MAILER_SERVER_IP      IN PTR    mailer.domain2.com.
YOUR_MAILER_SERVER_IP      IN PTR    mailer.domain3.com.
```

## Step 2: Gmail Setup

### 2.1 Enable 2-Factor Authentication

For each domain, you'll need a Gmail account:

1. Go to [Google Account Security](https://myaccount.google.com/security)
2. Enable 2-Step Verification
3. This is required for App Passwords

### 2.2 Generate App Passwords

For each domain, generate an App Password:

1. Go to [App Passwords](https://myaccount.google.com/apppasswords)
2. Select "Mail" or "Other (Custom name)"
3. Enter "ssmtp-mailer-domain1", "ssmtp-mailer-domain2", "ssmtp-mailer-domain3" as names
4. Copy the generated 16-character passwords
5. Save these passwords securely

**Required Gmail Accounts:**
- `mail-relay@domain1.com` → App Password: `ssmtp-mailer-domain1`
- `mail-relay@domain2.com` → App Password: `ssmtp-mailer-domain2`
- `mail-relay@domain3.com` → App Password: `ssmtp-mailer-domain3`

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
```

### 4.2 Domain Configuration

#### domain1.com Configuration

Create `/etc/ssmtp-mailer/domains/domain1.com.conf`:

```ini
[domain:domain1.com]
enabled = true
smtp_server = smtp.gmail.com
smtp_port = 587
auth_method = LOGIN
relay_account = mail-relay@domain1.com
username = mail-relay@domain1.com
password = YOUR_GMAIL_APP_PASSWORD_FOR_DOMAIN1
use_ssl = false
use_starttls = true
```

Create `/etc/ssmtp-mailer/domains/mailer.domain1.com.conf`:

```ini
[domain:mailer.domain1.com]
enabled = true
smtp_server = localhost
smtp_port = 25
auth_method = NONE
relay_account = mail-relay@mailer.domain1.com
use_ssl = false
use_starttls = false
```

#### domain2.com Configuration

Create `/etc/ssmtp-mailer/domains/domain2.com.conf`:

```ini
[domain:domain2.com]
enabled = true
smtp_server = smtp.gmail.com
smtp_port = 587
auth_method = LOGIN
relay_account = mail-relay@domain2.com
username = mail-relay@domain2.com
password = YOUR_GMAIL_APP_PASSWORD_FOR_DOMAIN2
use_ssl = false
use_starttls = true
```

Create `/etc/ssmtp-mailer/domains/mailer.domain2.com.conf`:

```ini
[domain:mailer.domain2.com]
enabled = true
smtp_server = localhost
smtp_port = 25
auth_method = NONE
relay_account = mail-relay@mailer.domain2.com
use_ssl = false
use_starttls = false
```

#### domain3.com Configuration

Create `/etc/ssmtp-mailer/domains/domain3.com.conf`:

```ini
[domain:domain3.com]
enabled = true
smtp_server = smtp.gmail.com
smtp_port = 587
auth_method = LOGIN
relay_account = mail-relay@domain3.com
username = mail-relay@domain3.com
password = YOUR_GMAIL_APP_PASSWORD_FOR_DOMAIN3
use_ssl = false
use_starttls = true
```

Create `/etc/ssmtp-mailer/domains/mailer.domain3.com.conf`:

```ini
[domain:mailer.domain3.com]
enabled = true
smtp_server = localhost
smtp_port = 25
auth_method = NONE
relay_account = mail-relay@mailer.domain3.com
use_ssl = false
use_starttls = false
```

### 4.3 User Configuration

#### domain1.com Users

Create `/etc/ssmtp-mailer/users/mail-relay@domain1.com.conf`:

```ini
[user:mail-relay@domain1.com]
enabled = true
domain = domain1.com
can_send_from = true
can_send_to = true
template_address = false
allowed_recipients = *@domain1.com
allowed_domains = domain1.com
```

#### domain2.com Users

Create `/etc/ssmtp-mailer/users/mail-relay@domain2.com.conf`:

```ini
[user:mail-relay@domain2.com]
enabled = true
domain = domain2.com
can_send_from = true
can_send_to = true
template_address = false
allowed_recipients = *@domain2.com
allowed_domains = domain2.com
```

#### domain3.com Users

Create `/etc/ssmtp-mailer/users/mail-relay@domain3.com.conf`:

```ini
[user:mail-relay@domain3.com]
enabled = true
domain = domain3.com
can_send_from = true
can_send_to = true
template_address = false
allowed_recipients = *@domain3.com
allowed_domains = domain3.com
```

### 4.4 Address Mappings

The mapping system works by defining patterns that automatically redirect emails from mailer subdomains to the main domains. Here's how it works:

#### Understanding the Mapping System

**Mapping Structure:**
- **from_pattern**: The email address on the mailer subdomain that receives emails
- **to_pattern**: The corresponding email address on the main domain where emails are relayed
- **smtp_account**: The Gmail account used to send the relayed email
- **domain**: The main domain for this mapping

**How It Works:**
1. Email sent to `contact-general@mailer.domain1.com`
2. System matches the `from_pattern` in mappings
3. Email is relayed using the specified `smtp_account`
4. Email is sent to `contact-general@domain1.com`

#### domain1.com Mappings

Create `/etc/ssmtp-mailer/mappings/domain1-mappings.conf`:

```ini
[mapping:contact-general-domain1]
from_pattern = contact-general@mailer.domain1.com
to_pattern = contact-general@domain1.com
smtp_account = mail-relay@domain1.com
domain = domain1.com

[mapping:contact-legal-domain1]
from_pattern = contact-legal@mailer.domain1.com
to_pattern = contact-legal@domain1.com
smtp_account = mail-relay@domain1.com
domain = domain1.com

[mapping:contact-privacy-domain1]
from_pattern = contact-privacy@mailer.domain1.com
to_pattern = contact-privacy@domain1.com
smtp_account = mail-relay@domain1.com
domain = domain1.com

[mapping:contact-support-domain1]
from_pattern = contact-support@mailer.domain1.com
to_pattern = contact-support@domain1.com
smtp_account = mail-relay@domain1.com
domain = domain1.com

[mapping:contact-sales-domain1]
from_pattern = contact-sales@mailer.domain1.com
to_pattern = contact-sales@domain1.com
smtp_account = mail-relay@domain1.com
domain = domain1.com
```

#### domain2.com Mappings

Create `/etc/ssmtp-mailer/mappings/domain2-mappings.conf`:

```ini
[mapping:contact-general-domain2]
from_pattern = contact-general@mailer.domain2.com
to_pattern = contact-general@domain2.com
smtp_account = mail-relay@domain2.com
domain = domain2.com

[mapping:contact-legal-domain2]
from_pattern = contact-legal@mailer.domain2.com
to_pattern = contact-legal@domain2.com
smtp_account = mail-relay@domain2.com
domain = domain2.com

[mapping:contact-privacy-domain2]
from_pattern = contact-privacy@mailer.domain2.com
to_pattern = contact-privacy@domain2.com
smtp_account = mail-relay@domain2.com
domain = domain2.com

[mapping:contact-support-domain2]
from_pattern = contact-support@mailer.domain2.com
to_pattern = contact-support@domain2.com
smtp_account = mail-relay@domain2.com
domain = domain2.com

[mapping:contact-sales-domain2]
from_pattern = contact-sales@mailer.domain2.com
to_pattern = contact-sales@domain2.com
smtp_account = mail-relay@domain2.com
domain = domain2.com
```

#### domain3.com Mappings

Create `/etc/ssmtp-mailer/mappings/domain3-mappings.conf`:

```ini
[mapping:contact-general-domain3]
from_pattern = contact-general@mailer.domain3.com
to_pattern = contact-general@domain3.com
smtp_account = mail-relay@domain3.com
domain = domain3.com

[mapping:contact-legal-domain3]
from_pattern = contact-legal@mailer.domain3.com
to_pattern = contact-legal@domain3.com
smtp_account = mail-relay@domain3.com
domain = domain3.com

[mapping:contact-privacy-domain3]
from_pattern = contact-privacy@mailer.domain3.com
to_pattern = contact-privacy@domain3.com
smtp_account = mail-relay@domain3.com
domain = domain3.com

[mapping:contact-support-domain3]
from_pattern = contact-support@mailer.domain3.com
to_pattern = contact-support@domain3.com
smtp_account = mail-relay@domain3.com
domain = domain3.com

[mapping:contact-sales-domain3]
from_pattern = contact-sales@mailer.domain3.com
to_pattern = contact-sales@domain3.com
smtp_account = mail-relay@domain3.com
domain = domain3.com
```

#### Advanced Mapping Examples

**Wildcard Mappings (catch-all):**
```ini
[mapping:catch-all-domain1]
from_pattern = *@mailer.domain1.com
to_pattern = *@domain1.com
smtp_account = mail-relay@domain1.com
domain = domain1.com
```

**Department Mappings:**
```ini
[mapping:hr-domain1]
from_pattern = hr@mailer.domain1.com
to_pattern = hr@domain1.com
smtp_account = mail-relay@domain1.com
domain = domain1.com

[mapping:it-domain1]
from_pattern = it@mailer.domain1.com
to_pattern = it@domain1.com
smtp_account = mail-relay@domain1.com
domain = domain1.com
```

**Individual User Mappings:**
```ini
[mapping:john-doe-domain1]
from_pattern = john.doe@mailer.domain1.com
to_pattern = john.doe@domain1.com
smtp_account = mail-relay@domain1.com
domain = domain1.com
```

## Step 5: Testing

### 5.1 Test Configuration

```bash
# Test the configuration
ssmtp-mailer config

# Expected output:
# Configuration Status:
#   Mailer configured: Yes
#   Domains loaded: 3 (domain1.com, domain2.com, domain3.com)
#   Users loaded: 3
#   Mappings loaded: 15
#   Status: Ready
```

### 5.2 Test SMTP Connection

```bash
# Test connection to Gmail for each domain
ssmtp-mailer test --domain domain1.com
ssmtp-mailer test --domain domain2.com
ssmtp-mailer test --domain domain3.com

# Expected output for each:
# Testing SMTP connection for domain1.com...
# Connection test successful!
```

### 5.3 Test Email Relay

#### Test domain1.com Relay

```bash
# Send a test email through domain1.com relay
ssmtp-mailer send \
  --from contact-general@mailer.domain1.com \
  --to test@example.com \
  --subject "Test Email from domain1.com Relay" \
  --body "This is a test email sent through the domain1.com relay system."

# Expected output:
# Email sent successfully!
# Message ID: <generated-message-id>
# Relay: contact-general@mailer.domain1.com → contact-general@domain1.com
```

#### Test domain2.com Relay

```bash
# Send a test email through domain2.com relay
ssmtp-mailer send \
  --from contact-support@mailer.domain2.com \
  --to test@example.com \
  --subject "Test Email from domain2.com Relay" \
  --body "This is a test email sent through the domain2.com relay system."
```

#### Test domain3.com Relay

```bash
# Send a test email through domain3.com relay
ssmtp-mailer send \
  --from contact-sales@mailer.domain3.com \
  --to test@example.com \
  --subject "Test Email from domain3.com Relay" \
  --body "This is a test email sent through the domain3.com relay system."
```

## Step 6: Production Deployment

### 6.1 Systemd Service

Create `/etc/systemd/system/ssmtp-mailer.service`:

```ini
[Unit]
Description=ssmtp-mailer Multi-Domain Relay Service
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

# Check domain-specific logs
sudo grep "domain1.com" /var/log/ssmtp-mailer.log
sudo grep "domain2.com" /var/log/ssmtp-mailer.log
sudo grep "domain3.com" /var/log/ssmtp-mailer.log
```

### 7.2 Monitor Service

```bash
# Check service status
sudo systemctl status ssmtp-mailer

# Check service logs
sudo journalctl -u ssmtp-mailer -f
```

### 7.3 Monitor Relay Activity

```bash
# Check successful relays
sudo grep "Relay successful" /var/log/ssmtp-mailer.log

# Check failed relays
sudo grep "Relay failed" /var/log/ssmtp-mailer.log

# Check domain statistics
ssmtp-mailer stats --domain domain1.com
ssmtp-mailer stats --domain domain2.com
ssmtp-mailer stats --domain domain3.com
```

## Troubleshooting

### Common Issues

1. **DNS Not Propagated**
   - Wait 24-48 hours for DNS propagation
   - Test with different DNS servers
   - Verify records are correct for all domains

2. **Gmail Authentication Failed**
   - Verify App Passwords are correct for each domain
   - Ensure 2FA is enabled on all Gmail accounts
   - Check accounts are not locked

3. **Connection Refused**
   - Check firewall settings
   - Verify server is listening on port 25
   - Check network connectivity

4. **Email Not Delivered**
   - Check SPF/DKIM/DMARC records for all domains
   - Verify reverse DNS for all mailer subdomains
   - Check Gmail sending limits for each account

5. **Wrong Domain Relay**
   - Verify mapping patterns are correct
   - Check domain configuration files
   - Ensure SMTP accounts match domains

### Debug Commands

```bash
# Test DNS resolution for all domains
nslookup mailer.domain1.com
nslookup mailer.domain2.com
nslookup mailer.domain3.com
nslookup smtp.gmail.com

# Test SMTP manually
telnet smtp.gmail.com 587

# Check configuration
ssmtp-mailer config

# Test connections for each domain
ssmtp-mailer test --domain domain1.com
ssmtp-mailer test --domain domain2.com
ssmtp-mailer test --domain domain3.com

# Verify mappings
ssmtp-mailer mappings --list
ssmtp-mailer mappings --test contact-general@mailer.domain1.com
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
   - Use different App Passwords for each domain

4. **Rate Limiting**
   - Enable rate limiting in configuration
   - Monitor for abuse across all domains

5. **Domain Isolation**
   - Each domain uses its own Gmail account
   - Mappings are domain-specific
   - No cross-domain email relay

## Maintenance

### Regular Tasks

1. **Update Credentials**
   - Rotate Gmail App Passwords quarterly for all domains
   - Update SSL certificates

2. **Monitor Logs**
   - Check for errors daily across all domains
   - Monitor disk space
   - Review relay statistics

3. **Backup Configuration**
   - Backup configuration files monthly
   - Test restore procedures
   - Document any configuration changes

### Updates

1. **Application Updates**
   - Keep mailer application updated
   - Test updates in staging with all domains

2. **System Updates**
   - Apply security patches
   - Test after major updates

3. **Domain Management**
   - Review and update mappings as needed
   - Add new contact addresses when required
   - Remove unused mappings

## Support

For issues:

1. Check logs: `/var/log/ssmtp-mailer.log`
2. Verify DNS configuration for all domains
3. Test SMTP connections manually for each domain
4. Check Gmail account status for all relay accounts
5. Review firewall and network settings
6. Verify mapping patterns and domain configurations
7. Test individual domain relays separately

### Getting Help

When reporting issues, include:
- Domain(s) affected
- Error messages from logs
- Configuration file contents (with passwords removed)
- DNS test results
- SMTP connection test results
