# SSL/TLS Quick Reference for ssmtp-mailer

Quick commands and configurations for SSL/TLS setup with Certbot and ssmtp-mailer.

## Quick Commands

### Install Certbot
```bash
# Ubuntu/Debian
sudo apt install certbot

# CentOS/RHEL
sudo yum install certbot

# macOS
brew install certbot
```

### Get SSL Certificate
```bash
# Standalone mode (recommended)
sudo certbot certonly --standalone \
  --email your-email@domain.com \
  --agree-tos \
  -d mail.yourdomain.com

# Webroot mode (if web server running)
sudo certbot certonly --webroot \
  --webroot-path /var/www/html \
  --email your-email@domain.com \
  --agree-tos \
  -d mail.yourdomain.com
```

### Check Certificates
```bash
# List all certificates
sudo certbot certificates

# Check specific certificate
sudo certbot show --cert-name mail.yourdomain.com

# Check expiration
openssl x509 -in /etc/letsencrypt/live/mail.yourdomain.com/fullchain.pem -noout -dates
```

### Renew Certificates
```bash
# Test renewal
sudo certbot renew --dry-run

# Force renewal
sudo certbot renew --force-renewal

# Manual renewal
sudo certbot renew
```

## Configuration Files

### ssmtp-mailer SSL Configuration
```ini
# /etc/ssmtp-mailer/ssmtp-mailer.conf
[smtp]
host = mail.yourdomain.com
port = 587
use_tls = true
use_starttls = true

[ssl]
certificate_file = /etc/letsencrypt/live/mail.yourdomain.com/fullchain.pem
private_key_file = /etc/letsencrypt/live/mail.yourdomain.com/privkey.pem
ca_bundle = /etc/letsencrypt/live/mail.yourdomain.com/chain.pem

[security]
verify_peer = true
verify_peer_name = true
```

### Certificate Renewal Script
```bash
#!/bin/bash
# /usr/local/bin/renew-ssl-certs.sh

certbot renew --quiet --no-self-upgrade

if [ $? -eq 0 ]; then
    if [ -f /var/lib/letsencrypt/.renewal_attempted ]; then
        echo "Certificates renewed, reloading ssmtp-mailer..."
        sudo systemctl reload ssmtp-mailer
    fi
fi
```

### Cron Job for Auto-renewal
```bash
# Add to crontab: sudo crontab -e
0 0,12 * * * /usr/local/bin/renew-ssl-certs.sh
```

## Testing Commands

### Test SSL Connection
```bash
# Test with ssmtp-mailer
ssmtp-mailer test --config /etc/ssmtp-mailer/ssmtp-mailer.conf

# Test with OpenSSL
openssl s_client -connect mail.yourdomain.com:587 -starttls smtp

# Test certificate validity
openssl x509 -in /etc/letsencrypt/live/mail.yourdomain.com/fullchain.pem -text -noout
```

### Test Certificate Chain
```bash
# Verify certificate chain
openssl verify /etc/letsencrypt/live/mail.yourdomain.com/fullchain.pem

# Check intermediate certificates
openssl x509 -in /etc/letsencrypt/live/mail.yourdomain.com/chain.pem -text -noout
```

## Troubleshooting

### Common Issues
```bash
# Permission denied
sudo chmod 640 /etc/letsencrypt/live/mail.yourdomain.com/privkey.pem
sudo chown root:ssmtp-mailer /etc/letsencrypt/live/mail.yourdomain.com/privkey.pem

# Certificate expired
sudo certbot renew --force-renewal

# Renewal failed
sudo tail -f /var/log/letsencrypt/letsencrypt.log

# SSL handshake failed
openssl s_client -connect mail.yourdomain.com:587 -starttls smtp -debug
```

### Debug Commands
```bash
# Enable verbose logging
ssmtp-mailer -V test --config /etc/ssmtp-mailer/ssmtp-mailer.conf

# Check system logs
sudo journalctl -u ssmtp-mailer -f

# Check certificate details
sudo certbot certificates --verbose
```

## Security Checklist

- [ ] Private key permissions set to 640
- [ ] Private key owned by root:ssmtp-mailer
- [ ] Certificate directory permissions set to 750
- [ ] Automatic renewal configured and tested
- [ ] Firewall rules configured for SMTP ports
- [ ] SSL configuration tested and verified
- [ ] Backup plan for certificate failures
- [ ] Monitoring and alerting configured

## Ports and Protocols

| Port | Protocol | Purpose |
|------|----------|---------|
| 25   | SMTP     | Standard SMTP (unencrypted) |
| 465  | SMTPS    | SMTP over SSL |
| 587  | SMTP     | SMTP with STARTTLS |

## Certificate Paths

```
/etc/letsencrypt/live/mail.yourdomain.com/
├── fullchain.pem    # Use this for certificate_file
├── privkey.pem      # Use this for private_key_file
├── cert.pem         # Certificate only
└── chain.pem        # Use this for ca_bundle
```

## Quick SSL Test

```bash
# One-liner to test SSL setup
echo "Testing SSL connection..." && \
openssl s_client -connect mail.yourdomain.com:587 -starttls smtp < /dev/null 2>/dev/null | \
grep -E "(CONNECTED|subject|issuer|verify|depth)" && \
echo "SSL test completed"
```

## Emergency Commands

```bash
# Force certificate renewal
sudo certbot renew --force-renewal

# Check certificate status
sudo certbot certificates

# View renewal logs
sudo tail -100 /var/log/letsencrypt/letsencrypt.log

# Test renewal process
sudo certbot renew --dry-run

# Manual certificate download
sudo certbot certonly --standalone --force-renewal -d mail.yourdomain.com
```

---

**Remember**: Always test in staging before production, and keep backups of your SSL configuration!
