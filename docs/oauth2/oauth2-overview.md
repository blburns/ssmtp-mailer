# OAuth2 Overview

## What is OAuth2?

OAuth2 (Open Authorization 2.0) is an industry-standard protocol for authorization that allows applications to access user resources without sharing user credentials. In the context of email services, OAuth2 enables your mail relay application to send emails on behalf of users without requiring their actual email passwords.

## 🔑 Key Concepts

### Authentication vs Authorization

- **Authentication**: Verifying who you are (username/password)
- **Authorization**: Granting permission to access resources (OAuth2 tokens)

### OAuth2 Roles

1. **Resource Owner**: The user (you) who owns the email account
2. **Client**: Your mail relay application (ssmtp-mailer)
3. **Authorization Server**: The email provider (Gmail, Office 365, etc.)
4. **Resource Server**: The email service that stores your emails

### OAuth2 Flow Types

#### Authorization Code Flow (What we use)
```
User → App → Provider → User → Provider → App → Tokens
```

#### Client Credentials Flow
```
App → Provider → Tokens (for service-to-service)
```

## 🔄 OAuth2 Authorization Code Flow

### Step 1: User Authorization Request
```
User clicks "Login with [Provider]"
↓
App redirects to Provider's authorization URL
↓
User sees consent screen
↓
User grants permission
↓
Provider redirects back with authorization code
```

### Step 2: Token Exchange
```
App receives authorization code
↓
App exchanges code for tokens
↓
Provider returns access token + refresh token
↓
App stores tokens securely
```

### Step 3: Using Tokens
```
App uses access token for API calls
↓
Token expires (usually 1 hour)
↓
App uses refresh token to get new access token
↓
Process continues automatically
```

## 🎯 Why OAuth2 for Email?

### Traditional Password Authentication Problems
- ❌ Passwords stored in plain text
- ❌ No granular permission control
- ❌ Difficult to revoke access
- ❌ Security risks if compromised
- ❌ No audit trail

### OAuth2 Benefits
- ✅ No password sharing
- ✅ Granular permissions (scopes)
- ✅ Easy access revocation
- ✅ Secure token-based authentication
- ✅ Comprehensive audit logging
- ✅ Industry standard security

## 🔐 OAuth2 Scopes

Scopes define what permissions your application requests:

### Common Email Scopes
- `https://mail.google.com/` - Full Gmail access
- `https://graph.microsoft.com/Mail.Send` - Send emails via Microsoft Graph
- `https://www.googleapis.com/auth/gmail.send` - Gmail send-only access
- `https://www.googleapis.com/auth/gmail.readonly` - Gmail read-only access

### Scope Best Practices
- **Request minimal scopes** needed for functionality
- **Avoid overly broad permissions**
- **Document why each scope is needed**
- **Review and update scopes regularly**

## 🏗️ OAuth2 Architecture in ssmtp-mailer

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   User/Admin   │    │  ssmtp-mailer    │    │  Email Provider │
│                 │    │                  │    │                 │
│ 1. Configure   │───▶│ 2. OAuth2 Flow   │───▶│ 3. Authorization│
│    OAuth2      │    │                  │    │                 │
│                 │    │ 4. Store Tokens  │◀───│ 4. Return      │
│ 5. Send Email  │◀───│ 5. Use Tokens    │    │    Tokens      │
│                 │    │ 6. Refresh as    │    │                 │
└─────────────────┘    │    needed        │    └─────────────────┘
                       └──────────────────┘
```

## 🔧 OAuth2 Components in ssmtp-mailer

### 1. OAuth2 Helper Tools
- **Python scripts** for each provider
- **Web-based interfaces** for easy setup
- **Automatic token management**

### 2. Configuration Files
- **Client ID and secret** storage
- **Token storage** and management
- **Provider-specific settings**

### 3. Token Management
- **Automatic refresh** handling
- **Secure storage** of credentials
- **Error handling** and retry logic

## 📱 OAuth2 Helper Tools

### Command Line Tools
```bash
# List all providers
python3 tools/oauth2-helper/oauth2-helper.py --list

# Launch specific provider
python3 tools/oauth2-helper/oauth2-helper.py gmail
python3 tools/oauth2-helper/oauth2-helper.py office365
```

### Web-Based Tools
- **HTML interfaces** for non-technical users
- **Local callback servers** for development
- **Cross-platform compatibility**

## 🔒 Security Considerations

### Token Security
- **Access tokens** expire quickly (1 hour)
- **Refresh tokens** are long-lived - keep secure
- **Store tokens** in secure locations
- **Use HTTPS** for all communications

### Application Security
- **Client secrets** must be kept confidential
- **Redirect URIs** should be restricted
- **State parameters** prevent CSRF attacks
- **Regular credential rotation**

### Production Considerations
- **Domain verification** required
- **HTTPS redirect URIs** mandatory
- **Rate limiting** may apply
- **Audit logging** available

## 🚀 Getting Started

### 1. Choose Your Provider
Select from our supported email services:
- Gmail / Google Workspace
- Office 365 / Microsoft Graph
- SendGrid
- Amazon SES
- ProtonMail
- Zoho Mail
- Mailgun
- Fastmail

### 2. Set Up OAuth2 Application
Follow the provider-specific guide to:
- Create OAuth2 application
- Configure redirect URIs
- Get client credentials

### 3. Use Helper Tools
Generate tokens using our automated tools:
```bash
python3 tools/oauth2-helper/oauth2-helper.py [provider]
```

### 4. Configure ssmtp-mailer
Use the generated tokens in your configuration.

## 📚 Next Steps

- **Read your provider's specific guide** for detailed setup
- **Use the helper tools** to generate tokens
- **Configure ssmtp-mailer** with OAuth2 authentication
- **Test your setup** with sample emails
- **Monitor authentication** and token refresh

## 🔗 Related Documentation

- **[OAuth2 Security Best Practices](oauth2-security.md)**
- **[OAuth2 Helper Tools](oauth2-helper-tools.md)**
- **[Token Management](token-management.md)**
- **[Troubleshooting Guide](oauth2-troubleshooting.md)**

---

*This overview provides the foundation for understanding OAuth2 authentication. For specific setup instructions, please refer to your email provider's guide.*
