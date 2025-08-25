# OAuth2 Troubleshooting Guide

## Overview

This guide helps you resolve common OAuth2 authentication issues when setting up ssmtp-mailer with various email providers. It covers error messages, debugging steps, and solutions for the most frequently encountered problems.

## 🚨 Common Error Categories

### 1. Redirect URI Mismatch Errors
### 2. Authentication/Authorization Failures
### 3. Token Exchange Issues
### 4. Permission/Scope Problems
### 5. Network and Connectivity Issues
### 6. Provider-Specific Errors

## 🔍 Diagnostic Steps

### Step 1: Check Basic Requirements
```bash
# Verify Python installation
python3 --version

# Check required packages
pip3 list | grep requests

# Verify network connectivity
curl -I https://google.com
```

### Step 2: Review Configuration
- **Client ID** matches provider configuration
- **Client Secret** is correct and not expired
- **Redirect URIs** match exactly
- **Scopes** are properly configured
- **Test users** are added (if required)

### Step 3: Check Provider Status
- **API services** are enabled
- **OAuth2 consent screen** is configured
- **Rate limits** haven't been exceeded
- **Service status** is operational

## 🚨 Redirect URI Mismatch Errors

### Error: "redirect_uri_mismatch"

#### Gmail/Google
```
Error: redirect_uri_mismatch
The redirect URI in the request, http://localhost:8080, does not match the ones authorized for the OAuth client.
```

**Causes:**
- Redirect URI in helper doesn't match Google Cloud Console
- Missing or incorrect redirect URI configuration
- Protocol mismatch (http vs https)

**Solutions:**
1. **Check Google Cloud Console**:
   - Go to **APIs & Services** → **Credentials**
   - Click on your OAuth2 client
   - **Authorized redirect URIs** section
   - Add: `http://localhost:8080`

2. **Verify helper configuration**:
   ```bash
   # Check what redirect URI the helper is using
   python3 tools/oauth2-helper/oauth2-helper.py gmail --debug
   ```

3. **Update redirect URIs**:
   ```
   http://localhost:8080
   http://localhost:8080/
   http://localhost:8080/oauth2callback
   ```

#### Office 365/Microsoft
```
Error: AADSTS50011: The reply URL specified in the request does not match the reply URLs configured for the application
```

**Solutions:**
1. **Check Azure Portal**:
   - **App registrations** → Your app → **Authentication**
   - **Platform configurations** → **Web**
   - **Redirect URIs**: Add `http://localhost:8080`

2. **Verify exact match**:
   - No trailing slashes unless configured
   - Protocol must match exactly
   - Port numbers must match

### Error: "invalid_redirect_uri"

**Causes:**
- Redirect URI format is invalid
- Unsupported protocol or port
- Provider-specific restrictions

**Solutions:**
1. **Use standard ports**:
   - `http://localhost:8080` (recommended)
   - `http://localhost:3000`
   - `http://127.0.0.1:8080`

2. **Check provider documentation** for supported formats

## 🔐 Authentication/Authorization Failures

### Error: "invalid_client"

#### Gmail/Google
```
Error: invalid_client
The OAuth client was not found.
```

**Causes:**
- Client ID is incorrect
- Client ID doesn't exist
- Client ID is from different project

**Solutions:**
1. **Verify Client ID**:
   - Copy from **Google Cloud Console** → **Credentials**
   - Check **Project ID** matches
   - Ensure no extra spaces or characters

2. **Check project selection**:
   - Verify correct Google Cloud project is selected
   - Check project has Gmail API enabled

#### Office 365/Microsoft
```
Error: AADSTS700016: Application with identifier was not found in the directory
```

**Solutions:**
1. **Verify Application ID**:
   - Copy from **Azure Portal** → **App registrations**
   - Check **Directory (tenant) ID** matches
   - Ensure app is in correct tenant

2. **Check app registration status**:
   - App is properly registered
   - No typos in Application ID

### Error: "access_denied"

#### Gmail/Google
```
Error: access_denied
The user denied access to the requested scope.
```

**Causes:**
- User denied permission
- OAuth consent screen not configured
- Test users not added
- Scope not properly configured

**Solutions:**
1. **Check OAuth consent screen**:
   - **APIs & Services** → **OAuth consent screen**
   - **Test users** section includes your email
   - **Scopes** include `https://mail.google.com/`

2. **Verify user permissions**:
   - User has 2FA enabled
   - User is not blocked by admin policies
   - User is in test users list (if app not verified)

#### Office 365/Microsoft
```
Error: AADSTS65001: The user or administrator has not consented to use the application
```

**Solutions:**
1. **Grant admin consent**:
   - **API permissions** → **Grant admin consent for [Organization]**
   - Check **Status** shows "Granted"

2. **Verify user assignment**:
   - **Enterprise applications** → Your app → **Users and groups**
   - User is assigned to the application

## 🔄 Token Exchange Issues

### Error: "invalid_grant"

#### Gmail/Google
```
Error: invalid_grant
The authorization code has expired or has already been used.
```

**Causes:**
- Authorization code expired (5 minutes)
- Code was already used
- Code was generated for different redirect URI

**Solutions:**
1. **Start fresh OAuth2 flow**:
   - Close helper and restart
   - Generate new authorization URL
   - Complete flow within 5 minutes

2. **Check redirect URI consistency**:
   - Same URI used for authorization and token exchange
   - No protocol or port mismatches

#### Office 365/Microsoft
```
Error: AADSTS70008: The provided authorization code has expired or is invalid
```

**Solutions:**
1. **Restart OAuth2 flow**:
   - Authorization codes expire quickly
   - Generate new authorization URL
   - Complete flow promptly

2. **Verify app configuration**:
   - **Authentication** → **Platform configurations** → **Web**
   - Redirect URIs are correctly configured

### Error: "invalid_request"

**Causes:**
- Missing required parameters
- Malformed request
- Unsupported grant type

**Solutions:**
1. **Check helper script**:
   - Ensure all required parameters are sent
   - Verify grant type is `authorization_code`
   - Check parameter formatting

2. **Update helper tools**:
   ```bash
   # Get latest version
   git pull origin main
   ```

## 🔐 Permission/Scope Problems

### Error: "insufficient_scope"

#### Gmail/Google
```
Error: insufficient_scope
The request requires higher privileges than provided by the access token.
```

**Causes:**
- Requested scope not granted
- Scope configuration mismatch
- Token doesn't have required permissions

**Solutions:**
1. **Check OAuth consent screen**:
   - **Scopes** section includes required permissions
   - `https://mail.google.com/` for full Gmail access
   - `https://www.googleapis.com/auth/gmail.send` for send-only

2. **Verify token scopes**:
   ```bash
   # Check token information
   curl -H "Authorization: Bearer YOUR_ACCESS_TOKEN" \
        "https://www.googleapis.com/oauth2/v1/tokeninfo"
   ```

#### Office 365/Microsoft
```
Error: Insufficient privileges to complete the operation
```

**Solutions:**
1. **Check API permissions**:
   - **API permissions** → **Microsoft Graph**
   - Required permissions are added:
     - `Mail.Send`
     - `Mail.ReadWrite`
     - `User.Read`

2. **Grant admin consent**:
   - **API permissions** → **Grant admin consent for [Organization]**

## 🌐 Network and Connectivity Issues

### Error: "Connection refused" or "Network unreachable"

**Causes:**
- Local callback server port conflict
- Firewall blocking connections
- Network configuration issues

**Solutions:**
1. **Check port availability**:
   ```bash
   # Check if port 8080 is in use
   lsof -ti:8080
   
   # Kill conflicting process
   lsof -ti:8080 | xargs kill -9
   
   # Or use different port
   export OAUTH2_PORT=9000
   ```

2. **Check firewall settings**:
   - Allow localhost connections
   - Check antivirus software
   - Verify network policies

3. **Test local connectivity**:
   ```bash
   # Test local server
   python3 -m http.server 8080
   # In another terminal
   curl http://localhost:8080
   ```

### Error: "SSL/TLS connection failed"

**Causes:**
- SSL certificate issues
- TLS version mismatch
- Network proxy interference

**Solutions:**
1. **Check SSL/TLS configuration**:
   ```bash
   # Test SSL connection
   openssl s_client -connect smtp.gmail.com:587 -starttls smtp
   ```

2. **Update Python packages**:
   ```bash
   pip3 install --upgrade requests urllib3
   ```

3. **Check system certificates**:
   ```bash
   # Update system CA certificates
   sudo update-ca-certificates
   ```

## 📧 Provider-Specific Errors

### Gmail/Google Specific

#### "Quota exceeded"
```
Error: quota_exceeded
Daily Limit Exceeded
```

**Solutions:**
1. **Check API quotas**:
   - **Google Cloud Console** → **APIs & Services** → **Dashboard**
   - **Quotas** section shows current usage
   - **Gmail API** quotas and limits

2. **Request quota increase**:
   - **Quotas** → **Edit quotas**
   - Submit request for higher limits

#### "User not found"
```
Error: user_not_found
The user account does not exist
```

**Solutions:**
1. **Verify email address**:
   - Check spelling and domain
   - Ensure account exists and is active
   - Check for typos in configuration

### Office 365/Microsoft Specific

#### "Conditional access policy"
```
Error: AADSTS50076: Due to a configuration change made by your administrator
```

**Solutions:**
1. **Check conditional access**:
   - Contact Azure administrator
   - Verify user meets policy requirements
   - Check location, device, or risk-based policies

2. **Use compliant device**:
   - Join device to Azure AD
   - Install required security software
   - Meet compliance requirements

#### "Multi-factor authentication required"
```
Error: AADSTS50076: Multi-factor authentication is required
```

**Solutions:**
1. **Enable MFA**:
   - **Azure Portal** → **Azure Active Directory** → **Users**
   - **Multi-factor authentication** → Enable for user

2. **Use app passwords** (if allowed):
   - Generate app-specific password
   - Use in place of regular password

## 🛠️ Debugging Tools

### Enable Verbose Logging
```bash
# Run helper with debug output
python3 tools/oauth2-helper/oauth2-helper.py gmail --verbose

# Check browser developer tools
# Network tab shows OAuth2 requests
# Console shows JavaScript errors
```

### Test API Endpoints
```bash
# Test Gmail API directly
curl -H "Authorization: Bearer YOUR_ACCESS_TOKEN" \
     "https://gmail.googleapis.com/gmail/v1/users/me/profile"

# Test Microsoft Graph API
curl -H "Authorization: Bearer YOUR_ACCESS_TOKEN" \
     "https://graph.microsoft.com/v1.0/me"
```

### Check Token Information
```bash
# Gmail token info
curl "https://www.googleapis.com/oauth2/v1/tokeninfo?access_token=YOUR_TOKEN"

# Office 365 token info (decode JWT)
# Use online JWT decoder or Python script
```

## 🔧 Advanced Troubleshooting

### Check Provider Logs
1. **Google Cloud Console**:
   - **APIs & Services** → **Dashboard**
   - **Gmail API** usage and errors
   - **Credentials** → **OAuth 2.0 Client IDs** → **Usage**

2. **Azure Portal**:
   - **Azure Active Directory** → **Sign-in logs**
   - **Enterprise applications** → Your app → **Sign-in logs**
   - **Audit logs** for configuration changes

### Verify Network Configuration
```bash
# Test DNS resolution
nslookup smtp.gmail.com
nslookup smtp.office365.com

# Test port connectivity
telnet smtp.gmail.com 587
telnet smtp.office365.com 587

# Check proxy settings
echo $http_proxy
echo $https_proxy
```

### Check System Resources
```bash
# Check available ports
netstat -tuln | grep :8080

# Check Python processes
ps aux | grep python

# Check system limits
ulimit -a
```

## 📞 Getting Help

### Before Asking for Help
1. **Collect error details**:
   - Full error message
   - Provider being used
   - Steps to reproduce
   - System information

2. **Check this guide** for similar issues
3. **Review provider documentation**
4. **Test with minimal configuration**

### Where to Get Help
1. **GitHub Issues**: [ssmtp-mailer repository](https://github.com/blburns/ssmtp-mailer/issues)
2. **Provider Support**: Official documentation and support channels
3. **Community Forums**: Stack Overflow, provider-specific forums
4. **Local System Administrator**: For enterprise environments

### Information to Include
```
Error: [Full error message]
Provider: [Gmail/Office 365/etc.]
Helper Tool: [Command used]
System: [OS, Python version]
Steps: [What you did]
Expected: [What should happen]
Actual: [What actually happened]
```

## 📋 Summary

Most OAuth2 issues can be resolved by:

1. ✅ **Verifying configuration** matches provider settings
2. ✅ **Checking redirect URIs** are exactly correct
3. ✅ **Ensuring proper permissions** and scopes
4. ✅ **Starting fresh OAuth2 flow** when needed
5. ✅ **Using debugging tools** to identify issues
6. ✅ **Checking provider status** and quotas

**Remember**: OAuth2 is a standard protocol, so most issues are configuration-related rather than code bugs. Take your time to verify each setting! 🚀

---

*For provider-specific setup instructions, see the individual provider guides in this documentation.*
