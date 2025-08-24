# DNS Configuration Guide for Mail Relay

## Required DNS Records

### Main Domain (dreamlikelabs.com)

```dns
# A record for website
dreamlikelabs.com.          IN A      YOUR_WEBSITE_IP

# MX record
dreamlikelabs.com.          IN MX 10  mail.dreamlikelabs.com.

# SPF record
dreamlikelabs.com.          IN TXT    "v=spf1 include:_spf.google.com ~all"

# DMARC record
_dmarc.dreamlikelabs.com.   IN TXT    "v=DMARC1; p=quarantine; rua=mailto:dmarc@dreamlikelabs.com"
```

### Mailer Subdomain (mailer.dreamlikelabs.com)

```dns
# A record pointing to your mailer server
mailer.dreamlikelabs.com.   IN A      YOUR_MAILER_SERVER_IP

# MX record for the mailer
mailer.dreamlikelabs.com.   IN MX 10  mailer.dreamlikelabs.com.

# SPF record for mailer
mailer.dreamlikelabs.com.   IN TXT    "v=spf1 ip4:YOUR_MAILER_SERVER_IP ~all"

# Reverse DNS (PTR) - important for deliverability
YOUR_MAILER_SERVER_IP      IN PTR    mailer.dreamlikelabs.com.
```

### SMTP Relay (Optional)

```dns
# CNAME to Gmail (for convenience)
smtp.dreamlikelabs.com.     IN CNAME  smtp.gmail.com.

# Or direct A records to Gmail (more reliable)
smtp.dreamlikelabs.com.     IN A      173.194.76.108
smtp.dreamlikelabs.com.     IN A      173.194.76.109
```

## DNS Provider Setup

### Cloudflare
1. Add domain to Cloudflare
2. Update nameservers at registrar
3. Add DNS records above
4. Enable "Proxy" for A records
5. Set SSL/TLS to "Full (strict)"

### AWS Route 53
1. Create hosted zone
2. Update nameservers
3. Add DNS records
4. Enable health checks

## Testing DNS

```bash
# Test A records
nslookup mailer.dreamlikelabs.com
nslookup smtp.dreamlikelabs.com

# Test MX records
nslookup -type=mx mailer.dreamlikelabs.com

# Test SPF records
nslookup -type=txt dreamlikelabs.com
```

## Gmail Setup

1. **Enable 2-Factor Authentication**
2. **Generate App Password**:
   - Go to Google Account > Security > App passwords
   - Select "Mail" or "Other (Custom name)"
   - Use generated password in configuration

## Configuration Files

### Domain Config (domains/dreamlikelabs.com.conf)
```ini
[domain:dreamlikelabs.com]
enabled = true
smtp_server = smtp.gmail.com
smtp_port = 587
auth_method = LOGIN
username = mail-relay@dreamlikelabs.com
password = your_gmail_app_password
use_starttls = true
```

### Address Mappings (mappings/relay-mappings.conf)
```ini
[mapping:contact-general]
from_pattern = contact-general@mailer.dreamlikelabs.com
to_pattern = contact-general@dreamlikelabs.com
smtp_account = mail-relay@dreamlikelabs.com
domain = dreamlikelabs.com
```

## Testing

```bash
# Test configuration
ssmtp-mailer config

# Test SMTP connection
ssmtp-mailer test

# Send test email
ssmtp-mailer send \
  --from contact-general@mailer.dreamlikelabs.com \
  --to test@example.com \
  --subject "Test" \
  --body "Test email"
```
