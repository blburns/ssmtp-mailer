# Getting Started with ssmtp-mailer

Welcome to ssmtp-mailer! This guide will help you get started with installation and basic configuration.

## 🚀 Quick Start Options

### Option 1: Package Installation (Recommended)
Install pre-built packages for your platform:
- **[Linux](../installation/linux.md)** - `.deb`, `.rpm`, or `.tar.gz` packages
- **[macOS](../installation/macos.md)** - `.dmg` or `.pkg` installers
- **[Windows](../installation/windows.md)** - Windows installer

### Option 2: Build from Source
Build ssmtp-mailer from source code:
- **[Development Guide](../development/README.md)** - Complete build instructions
- **[Build Scripts](../development/build-scripts.md)** - Automated build processes

## 🔧 Prerequisites

### System Requirements
- **Operating System**: Linux, macOS, or Windows
- **Architecture**: x86_64, ARM64 (Apple Silicon), or Universal
- **Memory**: 512MB RAM minimum, 2GB recommended
- **Storage**: 100MB available disk space
- **Network**: Internet access for OAuth2 authentication

### Software Dependencies
- **Python 3.6+** for OAuth2 helper tools
- **Modern web browser** for OAuth2 authentication
- **SSL certificates** for secure email transmission
- **Domain ownership** for production deployments

## 📋 Installation Checklist

### Before You Start
- [ ] **Choose your platform** (Linux, macOS, Windows)
- [ ] **Verify system requirements** are met
- [ ] **Install Python 3.6+** if not already present
- [ ] **Prepare domain** for production use
- [ ] **Choose email provider** (Gmail, Office 365, etc.)

### During Installation
- [ ] **Download** appropriate package for your platform
- [ ] **Install** ssmtp-mailer system-wide
- [ ] **Verify installation** with basic commands
- [ ] **Set up OAuth2** authentication
- [ ] **Configure email relay** settings

### After Installation
- [ ] **Test email sending** with sample messages
- [ ] **Configure SSL/TLS** for secure transmission
- [ ] **Set up monitoring** and logging
- [ ] **Document configuration** for future reference

## 🎯 Choose Your Path

### For Beginners
1. **Start with** [Quick Start Guide](quick-start.md)
2. **Choose platform** from [Installation Guides](../installation/README.md)
3. **Follow step-by-step** instructions
4. **Use OAuth2 helpers** for authentication

### For Developers
1. **Review** [Architecture Overview](../architecture/README.md)
2. **Build from source** using [Development Guide](../development/README.md)
3. **Customize configuration** for your needs
4. **Contribute** to the project

### For System Administrators
1. **Plan deployment** using [Architecture Guide](../architecture/README.md)
2. **Install on target systems** following [Installation Guides](../installation/README.md)
3. **Configure for production** using [Configuration Guides](../configuration/README.md)
4. **Set up monitoring** and maintenance procedures

### For Enterprise Users
1. **Review** [OAuth2 Security](../oauth2/oauth2-security.md) best practices
2. **Set up service accounts** using [Service Account Guide](../configuration/service-accounts.md)
3. **Configure domain-wide delegation** for Google Workspace
4. **Implement monitoring** and alerting systems

## 🔐 Authentication Options

### OAuth2 (Recommended)
- **Most secure** authentication method
- **No password storage** required
- **Automatic token refresh** handling
- **Granular permissions** control

### Service Accounts
- **Enterprise-grade** authentication
- **Domain-wide delegation** support
- **Fully automated** operation
- **Audit logging** and monitoring

### API Keys
- **Simple setup** for basic use
- **Provider-specific** implementation
- **Less secure** than OAuth2
- **Suitable for development**

## 📚 Next Steps

### 1. Installation
Choose your platform and follow the installation guide:
- **[Linux Installation](../installation/linux.md)**
- **[macOS Installation](../installation/macos.md)**
- **[Windows Installation](../installation/windows.md)**

### 2. OAuth2 Setup
Configure secure authentication:
- **[OAuth2 Overview](../oauth2/README.md)**
- **[Provider Setup Guides](../oauth2/README.md#email-provider-guides)**
- **[Security Best Practices](../oauth2/oauth2-security.md)**

### 3. Configuration
Set up your email relay system:
- **[Configuration Overview](../configuration/README.md)**
- **[SSL/TLS Setup](../configuration/ssl-setup.md)**
- **[DNS Configuration](../configuration/dns-setup.md)**

### 4. Architecture
Understand system design:
- **[Architecture Overview](../architecture/README.md)**
- **[Relay Setup](../architecture/relay-setup.md)**
- **[System Design](../architecture/relay-architecture.md)**

## 🆘 Getting Help

### Documentation
- **Check this guide** for common questions
- **Review troubleshooting** sections in each guide
- **Use search** to find specific topics

### Community Support
- **GitHub Issues**: Report bugs and request features
- **GitHub Discussions**: Ask questions and share solutions
- **Documentation**: Submit improvements and corrections

### Emergency Support
- **Check logs** for error messages
- **Verify configuration** settings
- **Test connectivity** to email providers
- **Review OAuth2 troubleshooting** for authentication issues

## 📋 Common Questions

### Q: Which platform should I choose?
**A**: Choose based on your existing infrastructure:
- **Linux**: Best for servers and production deployments
- **macOS**: Great for development and personal use
- **Windows**: Suitable for Windows-based environments

### Q: Do I need OAuth2?
**A**: OAuth2 is recommended for:
- **Production deployments** requiring security
- **Google Workspace** or Office 365 accounts
- **Multi-user systems** with audit requirements
- **Enterprise environments** with compliance needs

### Q: Can I use this in production?
**A**: Yes! ssmtp-mailer is designed for:
- **Production email relay** systems
- **High-volume** email sending
- **Enterprise** deployments
- **Mission-critical** email infrastructure

### Q: How do I get support?
**A**: Support options include:
- **Documentation**: Comprehensive guides and examples
- **GitHub Issues**: Bug reports and feature requests
- **Community**: User discussions and solutions
- **Self-help**: Troubleshooting guides and tools

---

*Ready to get started? Choose your platform from the [Installation Guides](../installation/README.md) and begin your ssmtp-mailer journey!*
