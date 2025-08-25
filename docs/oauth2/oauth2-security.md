# OAuth2 Security Best Practices

## Overview

This guide provides comprehensive security best practices for implementing and managing OAuth2 authentication in ssmtp-mailer. Following these practices ensures your OAuth2 implementation remains secure against common threats and vulnerabilities.

## 🚨 Security Threats and Risks

### Common Attack Vectors

#### 1. Token Theft
- **Man-in-the-middle attacks** on unencrypted connections
- **Cross-site request forgery (CSRF)** attacks
- **Token leakage** through logging or error messages
- **Browser storage vulnerabilities** (XSS attacks)

#### 2. Authorization Code Interception
- **Authorization code reuse** attacks
- **Code injection** in redirect URIs
- **Timing attacks** on token exchange
- **Replay attacks** on authorization requests

#### 3. Client Credential Compromise
- **Client secret exposure** in source code
- **Credential stuffing** attacks
- **Brute force** attempts on client secrets
- **Social engineering** to obtain credentials

#### 4. Scope Escalation
- **Privilege escalation** through scope manipulation
- **Overly broad permissions** granting excessive access
- **Scope creep** over time without review
- **Implicit scope** assumptions

## 🔒 Security Best Practices

### 1. Transport Security

#### HTTPS Enforcement
```bash
# Production redirect URIs must use HTTPS
https://yourdomain.com/oauth2callback  # ✅ Secure
http://localhost:8080                   # ✅ Development only
http://yourdomain.com/oauth2callback    # ❌ Insecure for production

# Verify SSL/TLS configuration
openssl s_client -connect yourdomain.com:443 -servername yourdomain.com
```

#### TLS Configuration
```bash
# Use strong TLS versions
# Minimum: TLS 1.2
# Recommended: TLS 1.3

# Test TLS configuration
nmap --script ssl-enum-ciphers -p 443 yourdomain.com

# Check for weak ciphers
sslyze --regular yourdomain.com:443
```

#### Certificate Management
```bash
# Use valid SSL certificates
# Let's Encrypt for free certificates
sudo certbot certonly --standalone -d yourdomain.com

# Check certificate validity
openssl x509 -in /etc/letsencrypt/live/yourdomain.com/cert.pem -text -noout

# Monitor certificate expiration
echo "0 0 1 * * /usr/bin/certbot renew --quiet" | sudo crontab -
```

### 2. Client Credential Security

#### Client Secret Protection
```bash
# Never commit secrets to version control
echo "*.env" >> .gitignore
echo "oauth2_tokens.json" >> .gitignore
echo "config/secrets.json" >> .gitignore

# Use environment variables
export GMAIL_CLIENT_SECRET="your-secret"
export OFFICE365_CLIENT_SECRET="your-secret"

# Or use secure configuration files
chmod 600 config/secrets.json
chown $USER:$USER config/secrets.json
```

#### Credential Rotation
```bash
# Regular rotation schedule
# Client secrets: Every 90 days
# Refresh tokens: Every 180 days
# Service account keys: Every 365 days

# Google Cloud Console
# APIs & Services → Credentials → OAuth 2.0 Client IDs
# Create new client secret, update application, delete old

# Azure Portal
# App registrations → Your app → Certificates & secrets
# Add new secret, update application, remove old
```

#### Access Control
```bash
# Restrict access to credential files
sudo chown ssmtp-mailer:ssmtp-mailer /etc/ssmtp-mailer/oauth2/
sudo chmod 700 /etc/ssmtp-mailer/oauth2/
sudo chmod 600 /etc/ssmtp-mailer/oauth2/*.json

# Use dedicated service account
sudo useradd -r -s /bin/false ssmtp-mailer
sudo chown ssmtp-mailer:ssmtp-mailer /etc/ssmtp-mailer/
```

### 3. Redirect URI Security

#### URI Validation
```bash
# Strict redirect URI validation
# Only allow specific, verified URIs

# Development
http://localhost:8080
http://localhost:8080/oauth2callback

# Production
https://yourdomain.com/oauth2callback
https://api.yourdomain.com/oauth2callback

# Never use wildcards
https://*.yourdomain.com/oauth2callback  # ❌ Insecure
```

#### URI Registration
```bash
# Google Cloud Console
# APIs & Services → Credentials → OAuth 2.0 Client IDs
# Authorized redirect URIs: Add only necessary URIs

# Azure Portal
# App registrations → Your app → Authentication
# Platform configurations → Web → Redirect URIs
```

#### State Parameter Usage
```python
import secrets
import hashlib

# Generate cryptographically secure state
state = secrets.token_urlsafe(32)
state_hash = hashlib.sha256(state.encode()).hexdigest()

# Store state with expiration
state_store[state_hash] = {
    'value': state,
    'expires': time.time() + 300  # 5 minutes
}

# Validate state parameter
def validate_state(state_param):
    state_hash = hashlib.sha256(state_param.encode()).hexdigest()
    if state_hash in state_store:
        stored = state_store[state_hash]
        if time.time() < stored['expires']:
            del state_store[state_hash]
            return True
    return False
```

### 4. Scope and Permission Management

#### Principle of Least Privilege
```bash
# Request minimal scopes needed
# Gmail: https://mail.google.com/ (full access)
# Gmail: https://www.googleapis.com/auth/gmail.send (send only)
# Gmail: https://www.googleapis.com/auth/gmail.readonly (read only)

# Office 365: Mail.Send (send emails)
# Office 365: Mail.ReadWrite (read/write emails)
# Office 365: User.Read (user profile)

# Avoid overly broad scopes
# ❌ https://www.googleapis.com/auth/userinfo.profile
# ❌ https://graph.microsoft.com/User.ReadWrite.All
```

#### Scope Documentation
```markdown
# Document required scopes
## Gmail OAuth2 Scopes

### Required Scopes
- `https://mail.google.com/` - Send and read emails
  - **Reason**: Core functionality for mail relay
  - **Risk**: Medium - access to all emails

### Optional Scopes
- `https://www.googleapis.com/auth/gmail.readonly` - Read emails only
  - **Reason**: Alternative for read-only operations
  - **Risk**: Low - read access only

## Scope Review Schedule
- **Monthly**: Review current scopes
- **Quarterly**: Audit scope usage
- **Annually**: Comprehensive scope review
```

#### Dynamic Scope Selection
```python
# Allow users to choose scope level
scopes = {
    'minimal': ['https://www.googleapis.com/auth/gmail.send'],
    'standard': ['https://mail.google.com/'],
    'full': ['https://mail.google.com/', 'https://www.googleapis.com/auth/userinfo.profile']
}

# User selects scope level
selected_scopes = scopes[user_preference]
```

### 5. Token Security

#### Token Storage
```bash
# Secure token storage
# File permissions: 600 (owner read/write only)
chmod 600 oauth2_tokens.json

# Directory permissions: 700 (owner read/write/execute only)
chmod 700 /etc/ssmtp-mailer/oauth2/

# Ownership: dedicated service account
sudo chown ssmtp-mailer:ssmtp-mailer /etc/ssmtp-mailer/oauth2/
```

#### Token Encryption
```bash
# Encrypt sensitive token files
gpg --symmetric --cipher-algo AES256 oauth2_tokens.json

# Decrypt when needed
gpg --decrypt oauth2_tokens.json.gpg > oauth2_tokens.json

# Use GPG agent for key management
echo 'use-agent' >> ~/.gnupg/gpg.conf
```

#### Token Validation
```python
import jwt
import time

def validate_token(token, client_id, issuer):
    try:
        # Decode and validate JWT token
        payload = jwt.decode(
            token,
            options={"verify_signature": False},  # For validation only
            algorithms=["RS256"]
        )
        
        # Check issuer
        if payload.get('iss') != issuer:
            return False
            
        # Check audience
        if payload.get('aud') != client_id:
            return False
            
        # Check expiration
        if payload.get('exp', 0) < time.time():
            return False
            
        return True
    except jwt.InvalidTokenError:
        return False
```

### 6. Network Security

#### Firewall Configuration
```bash
# Restrict OAuth2 callback server access
# Only allow localhost connections

# UFW firewall rules
sudo ufw allow from 127.0.0.1 to any port 8080
sudo ufw deny 8080

# iptables rules
sudo iptables -A INPUT -p tcp --dport 8080 -s 127.0.0.1 -j ACCEPT
sudo iptables -A INPUT -p tcp --dport 8080 -j DROP
```

#### Network Isolation
```bash
# Use dedicated network interface for OAuth2
# Configure separate network namespace
sudo ip netns add oauth2-ns
sudo ip link add veth0 type veth peer name veth1
sudo ip link set veth1 netns oauth2-ns

# Run OAuth2 server in isolated namespace
sudo ip netns exec oauth2-ns python3 oauth2-helper.py
```

#### Proxy and Load Balancer Security
```bash
# Configure reverse proxy with security headers
# nginx configuration
server {
    listen 443 ssl http2;
    server_name yourdomain.com;
    
    # Security headers
    add_header X-Frame-Options DENY;
    add_header X-Content-Type-Options nosniff;
    add_header X-XSS-Protection "1; mode=block";
    add_header Strict-Transport-Security "max-age=31536000; includeSubDomains";
    
    # OAuth2 callback
    location /oauth2callback {
        proxy_pass http://127.0.0.1:8080;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
    }
}
```

### 7. Monitoring and Auditing

#### Authentication Logging
```python
import logging
import json
from datetime import datetime

# Configure OAuth2 logging
oauth2_logger = logging.getLogger('oauth2')
oauth2_logger.setLevel(logging.INFO)

# Log OAuth2 events
def log_oauth2_event(event_type, user_id, provider, success, details=None):
    log_entry = {
        'timestamp': datetime.utcnow().isoformat(),
        'event_type': event_type,
        'user_id': user_id,
        'provider': provider,
        'success': success,
        'ip_address': get_client_ip(),
        'user_agent': get_user_agent(),
        'details': details or {}
    }
    
    oauth2_logger.info(json.dumps(log_entry))
    
    # Also log to security monitoring system
    if not success:
        security_logger.warning(f"OAuth2 failure: {log_entry}")
```

#### Security Monitoring
```bash
# Monitor OAuth2 authentication attempts
tail -f /var/log/oauth2.log | grep -E "(FAILURE|ERROR|WARNING)"

# Set up alerts for suspicious activity
# Monitor for:
# - Multiple failed attempts
# - Unusual IP addresses
# - Rapid token refresh
# - Scope escalation attempts

# Example monitoring script
#!/bin/bash
# Monitor OAuth2 failures
FAILURES=$(grep "FAILURE" /var/log/oauth2.log | wc -l)
if [ $FAILURES -gt 10 ]; then
    echo "High OAuth2 failure rate detected: $FAILURES failures"
    # Send alert
    curl -X POST "https://hooks.slack.com/services/YOUR_WEBHOOK" \
         -H "Content-type: application/json" \
         -d "{\"text\":\"OAuth2 security alert: $FAILURES failures detected\"}"
fi
```

#### Audit Trail
```bash
# Enable system auditing
sudo auditctl -w /etc/ssmtp-mailer/oauth2/ -p wa -k oauth2-access
sudo auditctl -w /var/log/oauth2.log -p wa -k oauth2-logs

# Review audit logs
ausearch -k oauth2-access
ausearch -k oauth2-logs

# Generate audit reports
aureport --auth --summary
```

### 8. Incident Response

#### Security Incident Procedures
```bash
# 1. Immediate Response
# - Isolate affected systems
# - Revoke compromised tokens
# - Block suspicious IP addresses

# 2. Investigation
# - Review authentication logs
# - Check for unauthorized access
# - Identify attack vector

# 3. Recovery
# - Generate new credentials
# - Update all systems
# - Monitor for further attacks

# 4. Post-Incident
# - Document incident details
# - Update security procedures
# - Conduct security review
```

#### Token Revocation
```bash
# Google OAuth2 token revocation
curl -X POST "https://oauth2.googleapis.com/revoke" \
     -d "token=YOUR_ACCESS_TOKEN"

# Microsoft OAuth2 token revocation
# Go to Azure Portal → App registrations → Your app → Authentication → Revoke tokens

# Revoke all tokens for a user
# Google: Revoke in Google Account settings
# Microsoft: Revoke in Azure AD user settings
```

#### Emergency Procedures
```bash
# Emergency token rotation script
#!/bin/bash
# Emergency OAuth2 credential rotation

echo "Starting emergency OAuth2 credential rotation..."

# 1. Revoke existing tokens
echo "Revoking existing tokens..."
# Add revocation commands for each provider

# 2. Generate new credentials
echo "Generating new credentials..."
python3 tools/oauth2-helper/oauth2-helper.py gmail
python3 tools/oauth2-helper/oauth2-helper.py office365

# 3. Update configuration
echo "Updating configuration..."
# Update config files with new tokens

# 4. Restart services
echo "Restarting services..."
sudo systemctl restart ssmtp-mailer

echo "Emergency rotation complete!"
```

## 🧪 Security Testing

### Vulnerability Assessment
```bash
# Test OAuth2 implementation
# Use OWASP ZAP or similar tools

# Test for common vulnerabilities:
# - CSRF protection
# - Token exposure
# - Redirect URI validation
# - Scope escalation
# - Token replay attacks

# Run security scan
zap-baseline.py -t https://yourdomain.com/oauth2callback
```

### Penetration Testing
```bash
# Test OAuth2 flow security
# - Authorization code interception
# - Token theft attempts
# - Client credential compromise
# - Scope manipulation

# Use specialized OAuth2 testing tools
# - OAuth2 Playground
# - Burp Suite OAuth2 extension
# - Custom OAuth2 testing scripts
```

### Security Headers Testing
```bash
# Test security headers
curl -I https://yourdomain.com/oauth2callback

# Verify headers present:
# - X-Frame-Options
# - X-Content-Type-Options
# - X-XSS-Protection
# - Strict-Transport-Security
# - Content-Security-Policy
```

## 📋 Security Checklist

### Implementation Security
- [ ] **HTTPS enforced** for all OAuth2 communications
- [ ] **Strong TLS configuration** (TLS 1.2+)
- [ ] **Valid SSL certificates** with proper expiration monitoring
- [ ] **Client secrets protected** and never committed to version control
- [ ] **Redirect URIs strictly validated** and limited
- [ ] **State parameters implemented** for CSRF protection
- [ ] **Minimal scopes requested** following principle of least privilege

### Token Security
- [ ] **Secure token storage** with proper file permissions
- [ ] **Token encryption** for sensitive environments
- [ ] **Automatic token refresh** implemented
- [ ] **Token validation** on all requests
- [ ] **Token expiration monitoring** and alerts

### Network Security
- [ ] **Firewall rules** restricting OAuth2 callback access
- [ ] **Network isolation** for OAuth2 servers
- [ ] **Reverse proxy security** with proper headers
- [ ] **Load balancer security** configuration

### Monitoring and Response
- [ ] **Comprehensive logging** of OAuth2 events
- [ ] **Security monitoring** and alerting
- [ ] **Audit trail** enabled and maintained
- [ ] **Incident response procedures** documented
- [ ] **Emergency procedures** for credential compromise

### Regular Maintenance
- [ ] **Credential rotation** on schedule
- [ ] **Security updates** applied promptly
- [ ] **Scope reviews** conducted regularly
- [ ] **Security assessments** performed periodically
- [ ] **Documentation updated** with security changes

## 🔗 Additional Resources

### Security Standards
- [OWASP OAuth2 Security Cheat Sheet](https://cheatsheetseries.owasp.org/cheatsheets/OAuth_2_0_Cheat_Sheet.html)
- [RFC 6819: OAuth2 Threat Model and Security Considerations](https://tools.ietf.org/html/rfc6819)
- [RFC 7636: PKCE for OAuth2](https://tools.ietf.org/html/rfc7636)

### Security Tools
- [OWASP ZAP](https://owasp.org/www-project-zap/) - Web application security scanner
- [Burp Suite](https://portswigger.net/burp) - Web application security testing
- [Nmap](https://nmap.org/) - Network security scanner
- [SSLyze](https://github.com/nabla-c0d3/sslyze) - SSL/TLS configuration scanner

### Security Monitoring
- [ELK Stack](https://www.elastic.co/what-is/elk-stack) - Log analysis and monitoring
- [Prometheus](https://prometheus.io/) - Metrics collection and alerting
- [Grafana](https://grafana.com/) - Visualization and monitoring dashboards

## 📋 Summary

OAuth2 security requires a comprehensive approach covering:

1. ✅ **Transport security** with HTTPS and strong TLS
2. ✅ **Credential protection** and regular rotation
3. ✅ **Strict validation** of redirect URIs and parameters
4. ✅ **Minimal scope** requests following least privilege
5. ✅ **Secure token storage** and management
6. ✅ **Network isolation** and access control
7. ✅ **Comprehensive monitoring** and incident response
8. ✅ **Regular security assessments** and updates

**Remember**: Security is not a one-time implementation but an ongoing process of monitoring, updating, and improving your OAuth2 implementation! 🔒

---

*For specific provider setup instructions, see the individual provider guides in this documentation.*
