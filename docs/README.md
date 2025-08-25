# ssmtp-mailer Documentation

Welcome to the comprehensive documentation for ssmtp-mailer, a powerful and secure email relay system with OAuth2 authentication support.

## 📚 Documentation Structure

### 🚀 **Getting Started**
- **[Quick Start Guide](getting-started/quick-start.md)** - Get up and running in minutes
- **[Prerequisites](getting-started/prerequisites.md)** - System requirements and dependencies
- **[Installation Overview](getting-started/README.md)** - Choose your installation method

### 💻 **Installation Guides**
- **[Linux Installation](installation/linux.md)** - Ubuntu, Debian, CentOS, RHEL
- **[macOS Installation](installation/macos.md)** - Homebrew and native installation
- **[Windows Installation](installation/windows.md)** - Windows 10/11 and WSL
- **[Installation Overview](installation/README.md)** - Platform comparison and selection

### ⚙️ **Configuration**
- **[Configuration Overview](configuration/README.md)** - Understanding configuration options
- **[SSL/TLS Setup](configuration/ssl-setup.md)** - Secure email with SSL certificates
- **[DNS Configuration](configuration/dns-setup.md)** - Email deliverability and SPF/DKIM
- **[Service Accounts](configuration/service-accounts.md)** - Google Workspace and OAuth2 setup

### 🏗️ **Architecture & Setup**
- **[Architecture Overview](architecture/README.md)** - System design and components
- **[Relay Architecture](architecture/relay-architecture.md)** - Email relay system design
- **[Relay Setup Guide](architecture/relay-setup.md)** - Complete relay configuration

### 🔐 **OAuth2 Authentication**
- **[OAuth2 Overview](oauth2/README.md)** - Complete OAuth2 authentication guide
- **[Provider Setup Guides](oauth2/README.md#email-provider-guides)** - Gmail, Office 365, SendGrid, and more
- **[Security Best Practices](oauth2/oauth2-security.md)** - OAuth2 security guidelines
- **[Token Management](oauth2/token-management.md)** - Managing OAuth2 tokens

### 🛠️ **Development & Building**
- **[Development Overview](development/README.md)** - Building from source
- **[Build Scripts](development/build-scripts.md)** - Automated build processes
- **[Building Guide](development/building.md)** - Manual build instructions

### 📖 **API Reference**
- **[API Overview](api/README.md)** - Programmatic interface documentation

## 🚀 Quick Start

### 1. Choose Your Platform
Select the installation guide for your operating system:
- **Linux**: [Linux Installation](installation/linux.md)
- **macOS**: [macOS Installation](installation/macos.md)
- **Windows**: [Windows Installation](installation/windows.md)

### 2. Install ssmtp-mailer
Follow the platform-specific installation guide to get ssmtp-mailer running on your system.

### 3. Configure OAuth2 (Recommended)
Set up secure OAuth2 authentication:
```bash
# List available OAuth2 providers
python3 tools/oauth2-helper/oauth2-helper.py --list

# Set up Gmail OAuth2
python3 tools/oauth2-helper/oauth2-helper.py gmail
```

### 4. Configure Email Relay
Follow the [relay setup guide](architecture/relay-setup.md) to configure your email relay system.

## 🔧 Prerequisites

- **Python 3.6+** for OAuth2 helper tools
- **Modern web browser** for OAuth2 authentication
- **Domain ownership** for production deployments
- **SSL certificates** for secure email transmission

## 📖 How to Use This Documentation

1. **Start with Getting Started** if you're new to ssmtp-mailer
2. **Choose your platform** from the installation guides
3. **Configure OAuth2** for secure authentication
4. **Set up email relay** following the architecture guides
5. **Reference configuration guides** for advanced setup

## 🆘 Getting Help

- **Check troubleshooting sections** in each guide
- **Review OAuth2 troubleshooting** for authentication issues
- **Use the helper tools** for automated setup
- **Open an issue** in our repository if you need assistance

## 🔗 Quick Links

### Popular Guides
- **[Gmail OAuth2 Setup](oauth2/gmail-oauth2-setup.md)** - Most popular OAuth2 provider
- **[Linux Installation](installation/linux.md)** - Most common deployment platform
- **[SSL Setup with Let's Encrypt](configuration/ssl-setup.md)** - Free SSL certificates
- **[Relay Setup](architecture/relay-setup.md)** - Complete email relay configuration

### Advanced Topics
- **[OAuth2 Security](oauth2/oauth2-security.md)** - Security best practices
- **[Service Account Setup](configuration/service-accounts.md)** - Enterprise authentication
- **[Architecture Design](architecture/relay-architecture.md)** - System design principles

## 📋 Documentation Status

| Section | Status | Last Updated |
|---------|--------|--------------|
| Getting Started | ✅ Complete | 2025-01-15 |
| Installation | ✅ Complete | 2025-01-15 |
| Configuration | ✅ Complete | 2025-01-15 |
| Architecture | ✅ Complete | 2025-01-15 |
| OAuth2 | ✅ Complete | 2025-01-15 |
| Development | ✅ Complete | 2025-01-15 |
| API Reference | 🔄 In Progress | 2025-01-15 |

## 🤝 Contributing

This documentation is maintained as part of the ssmtp-mailer project. To contribute:

1. **Fork the repository**
2. **Make your changes** to the documentation
3. **Submit a pull request** with clear descriptions
4. **Follow the existing style** and structure

## 📄 License

This documentation is licensed under the Apache License, Version 2.0. See the [LICENSE](../LICENSE) file for details.

---

*For updates and contributions, please visit our [GitHub repository](https://github.com/blburns/ssmtp-mailer).*
