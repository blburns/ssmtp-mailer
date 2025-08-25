# SSL/TLS Setup with Certbot for ssmtp-mailer

This guide explains how to set up SSL/TLS certificates using Certbot (Let's Encrypt) for secure email sending with ssmtp-mailer.

## Table of Contents

1. [Overview](#overview)
2. [Prerequisites](#prerequisites)
3. [Installing Certbot](#installing-certbot)
4. [Obtaining SSL Certificates](#obtaining-ssl-certificates)
5. [Configuring ssmtp-mailer with SSL](#configuring-ssmtp-mailer-with-ssl)
6. [Automating Certificate Renewal](#automating-certificate-renewal)
7. [Troubleshooting](#troubleshooting)
8. [Security Considerations](#security-considerations)

## Overview

ssmtp-mailer supports SSL/TLS encryption for secure email transmission. When combined with Certbot and Let's Encrypt, you can obtain free, trusted SSL certificates that automatically renew, ensuring your email communications remain secure.

**Benefits of using Certbot with ssmtp-mailer:**
- Free SSL certificates from Let's Encrypt
- Automatic certificate renewal
- Trusted by all major email providers
- No manual certificate management
- Industry-standard security practices

## Prerequisites

Before proceeding, ensure you have:

- A domain name pointing to your server
- Root or sudo access to your server
- Port 80 and 443 accessible for certificate validation
- ssmtp-mailer installed and configured
- Basic knowledge of Linux command line

## Installing Certbot

### Ubuntu/Debian
```bash
# Update package list
sudo apt update

# Install Certbot
sudo apt install certbot

# Verify installation
certbot --version
```

### CentOS/RHEL/Fedora
```bash
# Enable EPEL repository (CentOS/RHEL)
sudo yum install epel-release

# Install Certbot
sudo yum install certbot

# Or use dnf on newer versions
sudo dnf install certbot
```

### macOS
```bash
# Using Homebrew
brew install certbot

# Verify installation
certbot --version
```

### Manual Installation
```bash
# Download Certbot
wget https://dl.eff.org/certbot-auto
chmod a+x certbot-auto

# Run Certbot
./certbot-auto --help
```

## Obtaining SSL Certificates

### Standalone Mode (Recommended for email servers)
```bash
# Stop any services using port 80/443
sudo systemctl stop nginx  # if running
sudo systemctl stop apache2  # if running

# Obtain certificate
sudo certbot certonly --standalone \
  --email your-email@domain.com \
  --agree-tos \
  --no-eff-email \
  -d mail.yourdomain.com \
  -d yourdomain.com

# Verify certificate
sudo certbot certificates
```

### Webroot Mode (if you have a web server)
```bash
# Create webroot directory
sudo mkdir -p /var/www/html/.well-known/acme-challenge

# Obtain certificate
sudo certbot certonly --webroot \
  --webroot-path /var/www/html \
  --email your-email@domain.com \
  --agree-tos \
  --no-eff-email \
  -d mail.yourdomain.com \
  -d yourdomain.com
```

### DNS Challenge (for internal servers)
```bash
# Install DNS plugin (example for Cloudflare)
sudo apt install python3-certbot-dns-cloudflare

# Obtain certificate using DNS challenge
sudo certbot certonly --dns-cloudflare \
  --dns-cloudflare-credentials ~/.secrets/cloudflare.ini \
  --email your-email@domain.com \
  --agree-tos \
  --no-eff-email \
  -d mail.yourdomain.com
```

## Configuring ssmtp-mailer with SSL

### 1. Certificate Paths
After obtaining certificates, note these paths:
```bash
# Certificate files location
/etc/letsencrypt/live/mail.yourdomain.com/
├── fullchain.pem    # Full certificate chain
├── privkey.pem      # Private key
├── cert.pem         # Certificate only
└── chain.pem        # Intermediate certificates
```

### 2. Update ssmtp-mailer Configuration
Edit your ssmtp-mailer configuration file:

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

### 3. Set Proper Permissions
```bash
# Create ssmtp-mailer group if it doesn't exist
sudo groupadd ssmtp-mailer

# Add your user to the group
sudo usermod -a -G ssmtp-mailer $USER

# Set proper permissions for certificate files
sudo chmod 640 /etc/letsencrypt/live/mail.yourdomain.com/privkey.pem
sudo chown root:ssmtp-mailer /etc/letsencrypt/live/mail.yourdomain.com/privkey.pem

# Allow group read access to certificate directory
sudo chmod 750 /etc/letsencrypt/live/mail.yourdomain.com/
sudo chown root:ssmtp-mailer /etc/letsencrypt/live/mail.yourdomain.com/
```

### 4. Test SSL Configuration
```bash
# Test SSL connection
ssmtp-mailer test --config /etc/ssmtp-mailer/ssmtp-mailer.conf

# Test with verbose output
ssmtp-mailer -V test --config /etc/ssmtp-mailer/ssmtp-mailer.conf
```

## Automating Certificate Renewal

### 1. Test Renewal Process
```bash
# Test renewal without actually renewing
sudo certbot renew --dry-run

# Check renewal status
sudo certbot renew --dry-run --verbose
```

### 2. Set Up Automatic Renewal
```bash
# Create renewal script
sudo nano /usr/local/bin/renew-ssl-certs.sh
```

Add this content:
```bash
#!/bin/bash

# Renew SSL certificates
certbot renew --quiet --no-self-upgrade

# Reload ssmtp-mailer service if certificates were renewed
if [ $? -eq 0 ]; then
    # Check if certificates were actually renewed
    if [ -f /var/lib/letsencrypt/.renewal_attempted ]; then
        echo "Certificates renewed, reloading ssmtp-mailer..."
        sudo systemctl reload ssmtp-mailer
        # Or restart if reload is not supported
        # sudo systemctl restart ssmtp-mailer
    fi
fi
```

Make it executable:
```bash
sudo chmod +x /usr/local/bin/renew-ssl-certs.sh
```

### 3. Set Up Cron Job
```bash
# Edit crontab
sudo crontab -e

# Add this line to run twice daily
0 0,12 * * * /usr/local/bin/renew-ssl-certs.sh
```

### 4. Systemd Timer (Alternative to Cron)
```bash
# Create timer file
sudo nano /etc/systemd/system/ssl-renewal.timer
```

Add this content:
```ini
[Unit]
Description=SSL Certificate Renewal Timer
After=network.target

[Timer]
OnCalendar=*-*-* 00:00:00
OnCalendar=*-*-* 12:00:00
Persistent=true

[Install]
WantedBy=timers.target
```

Create service file:
```bash
sudo nano /etc/systemd/system/ssl-renewal.service
```

Add this content:
```ini
[Unit]
Description=SSL Certificate Renewal Service
After=network.target

[Service]
Type=oneshot
ExecStart=/usr/local/bin/renew-ssl-certs.sh
User=root

[Install]
WantedBy=multi-user.target
```

Enable and start the timer:
```bash
sudo systemctl enable ssl-renewal.timer
sudo systemctl start ssl-renewal.timer
sudo systemctl status ssl-renewal.timer
```

## Troubleshooting

### Common Issues and Solutions

#### 1. Permission Denied Errors
```bash
# Check certificate permissions
ls -la /etc/letsencrypt/live/mail.yourdomain.com/

# Fix permissions if needed
sudo chmod 640 /etc/letsencrypt/live/mail.yourdomain.com/privkey.pem
sudo chown root:ssmtp-mailer /etc/letsencrypt/live/mail.yourdomain.com/privkey.pem
```

#### 2. Certificate Validation Failures
```bash
# Check certificate validity
sudo certbot certificates

# Verify certificate chain
openssl x509 -in /etc/letsencrypt/live/mail.yourdomain.com/fullchain.pem -text -noout

# Check certificate expiration
openssl x509 -in /etc/letsencrypt/live/mail.yourdomain.com/fullchain.pem -noout -dates
```

#### 3. SSL Handshake Failures
```bash
# Test SSL connection manually
openssl s_client -connect mail.yourdomain.com:587 -starttls smtp

# Check for certificate chain issues
openssl s_client -connect mail.yourdomain.com:587 -starttls smtp -CAfile /etc/letsencrypt/live/mail.yourdomain.com/chain.pem
```

#### 4. Renewal Failures
```bash
# Check renewal logs
sudo tail -f /var/log/letsencrypt/letsencrypt.log

# Manually renew certificates
sudo certbot renew --force-renewal

# Check renewal configuration
sudo certbot show --cert-name mail.yourdomain.com
```

### Debug Mode
```bash
# Enable debug logging in ssmtp-mailer
ssmtp-mailer -V test --config /etc/ssmtp-mailer/ssmtp-mailer.conf

# Check system logs
sudo journalctl -u ssmtp-mailer -f

# Check SSL/TLS handshake
openssl s_client -connect mail.yourdomain.com:587 -starttls smtp -debug
```

## Security Considerations

### 1. Certificate Security
- Keep private keys secure and restrict access
- Use strong file permissions (640 for private keys)
- Regularly audit certificate access
- Monitor certificate expiration dates

### 2. Network Security
- Use firewall rules to restrict access to SMTP ports
- Consider using VPN for remote administration
- Monitor for unauthorized access attempts
- Use strong authentication methods

### 3. Best Practices
- Regularly update Certbot and ssmtp-mailer
- Monitor certificate renewal logs
- Have a backup plan for certificate failures
- Test SSL configuration after any changes
- Document your SSL setup and renewal process

### 4. Monitoring and Alerting
```bash
# Create monitoring script
sudo nano /usr/local/bin/monitor-ssl-certs.sh
```

Add this content:
```bash
#!/bin/bash

# Check certificate expiration
CERT_FILE="/etc/letsencrypt/live/mail.yourdomain.com/fullchain.pem"
DAYS_UNTIL_EXPIRY=$(openssl x509 -in "$CERT_FILE" -checkend 86400 -noout && echo "OK" || echo "EXPIRES_SOON")

if [ "$DAYS_UNTIL_EXPIRY" != "OK" ]; then
    echo "WARNING: SSL certificate expires soon!" | mail -s "SSL Certificate Warning" admin@yourdomain.com
fi
```

## Advanced Configuration

### 1. Multiple Domains
```bash
# Obtain certificate for multiple domains
sudo certbot certonly --standalone \
  --email your-email@domain.com \
  --agree-tos \
  --no-eff-email \
  -d mail.yourdomain.com \
  -d smtp.yourdomain.com \
  -d yourdomain.com
```

### 2. Wildcard Certificates
```bash
# Note: Requires DNS challenge
sudo certbot certonly --dns-cloudflare \
  --dns-cloudflare-credentials ~/.secrets/cloudflare.ini \
  --email your-email@domain.com \
  --agree-tos \
  --no-eff-email \
  -d *.yourdomain.com
```

### 3. Custom Certificate Paths
```bash
# Use custom paths in configuration
[ssl]
certificate_file = /opt/ssl/certs/fullchain.pem
private_key_file = /opt/ssl/private/privkey.pem
ca_bundle = /opt/ssl/certs/chain.pem
```

## Conclusion

Using Certbot with ssmtp-mailer provides a robust, secure, and cost-effective solution for SSL/TLS email encryption. The automatic renewal process ensures your certificates never expire, while the trusted Let's Encrypt CA ensures compatibility with all major email providers.

For additional support or questions, refer to:
- [Certbot Documentation](https://certbot.eff.org/docs/)
- [Let's Encrypt Documentation](https://letsencrypt.org/docs/)
- [ssmtp-mailer Documentation](docs/)
- [GitHub Issues](https://github.com/your-repo/ssmtp-mailer/issues)

---

**Note**: This documentation assumes you have administrative access to your server. Always test configurations in a staging environment before applying to production systems.
