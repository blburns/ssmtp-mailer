# Mail Relay Architecture & DNS Configuration Guide

## Overview

This document describes the mail relay architecture for `domain1.com` where a local mailer server acts as a relay, forwarding emails from local aliases to the main domain through Gmail SMTP.

## Architecture Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                    Internet Users                                │
└─────────────────────┬───────────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────────────┐
│              DNS Resolution                                     │
│  contact-general@mailer.domain1.com                      │
│  contact-legal@mailer.domain1.com                        │
│  contact-privacy@mailer.domain1.com                      │
│  contact-support@mailer.domain1.com                      │
│  contact-sales@mailer.domain1.com                        │
└─────────────────────┬───────────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────────────┐
│              Local Mailer Server                                │
│              mailer.domain1.com                           │
│                                                                 │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │              Address Mapping                             │   │
│  │  contact-general@mailer.domain1.com               │   │
│  │  └─→ contact-general@domain1.com                 │   │
│  │                                                         │   │
│  │  contact-legal@mailer.domain1.com                 │   │
│  │  └─→ contact-legal@domain1.com                   │   │
│  │                                                         │   │
│  │  contact-privacy@mailer.domain1.com               │   │
│  │  └─→ contact-privacy@domain1.com                 │   │
│  │                                                         │   │
│  │  contact-support@mailer.domain1.com               │   │
│  │  └─→ contact-support@domain1.com                 │   │
│  │                                                         │   │
│  │  contact-sales@mailer.domain1.com                 │   │
│  │  └─→ contact-sales@domain1.com                   │   │
│  └─────────────────────────────────────────────────────────┘   │
└─────────────────────┬───────────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────────────┐
│              Gmail SMTP Relay                                   │
│              smtp.gmail.com:587                                │
│              (via mail-relay@domain1.com)                │
└─────────────────────┬───────────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────────────┐
│              Final Recipients                                   │
│              @domain1.com                                │
└─────────────────────────────────────────────────────────────────┘
```

## DNS Configuration

### Required DNS Records

#### 1. Main Domain Records (domain1.com)

```dns
# A record for the main website
domain1.com.          IN A      YOUR_WEBSITE_IP

# MX record for receiving emails (if needed)
domain1.com.          IN MX 10  mail.domain1.com.

# SPF record for email authentication
domain1.com.          IN TXT    "v=spf1 include:_spf.google.com ~all"

# DKIM record (if using Gmail with custom domain)
google._domainkey.domain1.com. IN TXT "v=DKIM1; k=rsa; p=YOUR_DKIM_KEY"

# DMARC record for email policy
_dmarc.domain1.com.   IN TXT    "v=DMARC1; p=quarantine; rua=mailto:dmarc@domain1.com"
```

#### 2. Mailer Subdomain Records (mailer.domain1.com)

```dns
# A record pointing to your mailer server
mailer.domain1.com.   IN A      YOUR_MAILER_SERVER_IP

# MX record for the mailer subdomain
mailer.domain1.com.   IN MX 10  mailer.domain1.com.

# SPF record for the mailer subdomain
mailer.domain1.com.   IN TXT    "v=spf1 ip4:YOUR_MAILER_SERVER_IP ~all"

# Reverse DNS (PTR) record (important for email deliverability)
YOUR_MAILER_SERVER_IP      IN PTR    mailer.domain1.com.
```

#### 3. SMTP Relay Records

```dns
# CNAME record for SMTP relay (optional, for convenience)
smtp.domain1.com.     IN CNAME  smtp.gmail.com.

# Alternative: Direct A record to Gmail (more reliable)
smtp.domain1.com.     IN A      173.194.76.108
smtp.domain1.com.     IN A      173.194.76.109
```

### DNS Provider Setup

#### Cloudflare
1. Add your domain to Cloudflare
2. Update nameservers at your domain registrar
3. Add the DNS records above
4. Enable "Proxy" for A records (orange cloud)
5. Set SSL/TLS encryption mode to "Full (strict)"

#### AWS Route 53
1. Create a hosted zone for your domain
2. Update nameservers at your domain registrar
3. Add the DNS records above
4. Enable health checks for critical records

#### GoDaddy/Other Registrars
1. Access DNS management
2. Add the required records
3. Ensure TTL values are reasonable (300-3600 seconds)

## Configuration Files

### 1. Main Configuration (`ssmtp-mailer.conf`)

```ini
[global]
# Default hostname for SMTP connections
default_hostname = mailer.domain1.com

# Default from address when none specified
default_from = noreply@mailer.domain1.com

# Configuration directory paths
config_dir = /etc/ssmtp-mailer
domains_dir = /etc/ssmtp-mailer/domains
users_dir = /etc/ssmtp-mailer/users
mappings_dir = /etc/ssmtp-mailer/mappings
ssl_dir = /etc/ssmtp-mailer/ssl

# Logging configuration
log_file = /var/log/ssmtp-mailer.log
log_level = INFO

# Connection settings
max_connections = 10
connection_timeout = 30
read_timeout = 60
write_timeout = 60

# Rate limiting
enable_rate_limiting = true
rate_limit_per_minute = 100
```

### 2. Domain Configuration (`domains/domain1.com.conf`)

```ini
[domain:domain1.com]
enabled = true
smtp_server = smtp.gmail.com
smtp_port = 587
auth_method = LOGIN
relay_account = mail-relay@domain1.com
username = mail-relay@domain1.com
password = your_gmail_app_password_here
use_ssl = false
use_starttls = true
```

### 3. Local Domain Configuration (`domains/mailer.domain1.com.conf`)

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

### 4. Address Mappings (`mappings/relay-mappings.conf`)

```ini
[mapping:contact-general]
from_pattern = contact-general@mailer.domain1.com
to_pattern = contact-general@domain1.com
smtp_account = mail-relay@domain1.com
domain = domain1.com

[mapping:contact-legal]
from_pattern = contact-legal@mailer.domain1.com
to_pattern = contact-legal@domain1.com
smtp_account = mail-relay@domain1.com
domain = domain1.com

[mapping:contact-privacy]
from_pattern = contact-privacy@mailer.domain1.com
to_pattern = contact-privacy@domain1.com
smtp_account = mail-relay@domain1.com
domain = domain1.com

[mapping:contact-support]
from_pattern = contact-support@mailer.domain1.com
to_pattern = contact-support@domain1.com
smtp_account = mail-relay@domain1.com
domain = domain1.com

[mapping:contact-sales]
from_pattern = contact-sales@mailer.domain1.com
to_pattern = contact-sales@domain1.com
smtp_account = mail-relay@domain1.com
domain = domain1.com
```

### 5. User Configuration (`users/mail-relay@domain1.com.conf`)

```ini
[user:mail-relay@domain1.com]
enabled = true
domain = mailer.domain1.com
can_send_from = true
can_send_to = true
template_address = false
allowed_domains = domain1.com
```

## Gmail Setup

### 1. Enable 2-Factor Authentication
1. Go to [Google Account Security](https://myaccount.google.com/security)
2. Enable 2-Step Verification
3. This is required for App Passwords

### 2. Generate App Password
1. Go to [App Passwords](https://myaccount.google.com/apppasswords)
2. Select "Mail" or "Other (Custom name)"
3. Enter "ssmtp-mailer" as the name
4. Copy the generated 16-character password
5. Use this password in your configuration

### 3. Alternative: OAuth2 (More Secure)
1. Go to [Google Cloud Console](https://console.cloud.google.com/)
2. Create a new project
3. Enable Gmail API
4. Create OAuth 2.0 credentials
5. Use the access token in your configuration

## Testing the Setup

### 1. Test DNS Resolution
```bash
# Test A records
nslookup mailer.domain1.com
nslookup smtp.domain1.com

# Test MX records
nslookup -type=mx mailer.domain1.com
nslookup -type=mx domain1.com

# Test SPF records
nslookup -type=txt domain1.com
nslookup -type=txt mailer.domain1.com
```

### 2. Test SMTP Connection
```bash
# Test Gmail SMTP connection
telnet smtp.gmail.com 587

# Test local mailer
telnet mailer.domain1.com 25
```

### 3. Test Email Relay
```bash
# Test with the mailer application
ssmtp-mailer test

# Send a test email
ssmtp-mailer send \
  --from contact-general@mailer.domain1.com \
  --to test@example.com \
  --subject "Test Email" \
  --body "This is a test email from the relay system"
```

## Security Considerations

### 1. Firewall Configuration
```bash
# Allow SMTP traffic
ufw allow 25/tcp    # SMTP
ufw allow 587/tcp   # SMTP with STARTTLS
ufw allow 465/tcp   # SMTP with SSL

# Restrict access to mailer server
ufw allow from YOUR_TRUSTED_IPS to any port 25
```

### 2. SSL/TLS Configuration
- Always use STARTTLS for Gmail connections
- Verify SSL certificates
- Use strong cipher suites

### 3. Authentication
- Use App Passwords, not regular passwords
- Consider OAuth2 for production
- Rotate credentials regularly

### 4. Rate Limiting
- Enable rate limiting in configuration
- Monitor for abuse
- Set reasonable limits (100 emails/minute)

## Monitoring & Logging

### 1. Log Files
- `/var/log/ssmtp-mailer.log` - Application logs
- `/var/log/mail.log` - System mail logs
- `/var/log/auth.log` - Authentication logs

### 2. Key Metrics to Monitor
- Email delivery success rate
- SMTP connection failures
- Authentication failures
- Rate limiting triggers
- Disk space usage

### 3. Alerting
- Set up monitoring for critical failures
- Monitor disk space and log rotation
- Alert on authentication failures

## Troubleshooting

### Common Issues

1. **DNS Resolution Problems**
   - Verify DNS records are propagated
   - Check TTL values
   - Test with different DNS servers

2. **SMTP Connection Failures**
   - Check firewall settings
   - Verify Gmail credentials
   - Test network connectivity

3. **Authentication Failures**
   - Verify App Password is correct
   - Check 2FA is enabled
   - Ensure account is not locked

4. **Email Delivery Issues**
   - Check SPF/DKIM/DMARC records
   - Verify reverse DNS
   - Check Gmail sending limits

### Debug Commands
```bash
# Check mailer status
ssmtp-mailer config

# Test SMTP connection
ssmtp-mailer test

# View logs
tail -f /var/log/ssmtp-mailer.log

# Check DNS
dig mailer.domain1.com
dig smtp.domain1.com

# Test SMTP manually
telnet smtp.gmail.com 587
```

## Performance Optimization

### 1. Connection Pooling
- Reuse SMTP connections when possible
- Implement connection pooling
- Monitor connection usage

### 2. Caching
- Cache DNS lookups
- Cache authentication tokens
- Implement result caching

### 3. Async Processing
- Process emails asynchronously
- Use worker threads
- Implement queue management

## Backup & Recovery

### 1. Configuration Backup
```bash
# Backup configuration
tar -czf ssmtp-mailer-config-$(date +%Y%m%d).tar.gz /etc/ssmtp-mailer/

# Backup logs
tar -czf ssmtp-mailer-logs-$(date +%Y%m%d).tar.gz /var/log/ssmtp-mailer/
```

### 2. Recovery Procedures
1. Restore configuration files
2. Verify DNS settings
3. Test SMTP connections
4. Monitor email delivery

## Support & Maintenance

### 1. Regular Maintenance
- Update SSL certificates
- Rotate authentication credentials
- Monitor log file sizes
- Check system resources

### 2. Updates
- Keep the mailer application updated
- Monitor for security patches
- Test updates in staging environment

### 3. Documentation
- Keep this guide updated
- Document any customizations
- Maintain runbooks for common tasks
