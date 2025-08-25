# Installation Guide

Choose the right installation method for your platform and get ssmtp-mailer up and running quickly.

## 🚀 Installation Methods

### 1. Package Installation (Recommended)
**Best for**: Most users, production deployments, quick setup

- **Linux**: `.deb`, `.rpm`, or `.tar.gz` packages
- **macOS**: `.dmg` or `.pkg` installers
- **Windows**: Windows installer

**Pros**: Fast, reliable, pre-built, easy updates
**Cons**: Less customization, platform-specific

### 2. Build from Source
**Best for**: Developers, custom builds, latest features

- **Cross-platform**: CMake-based build system
- **Customization**: Compiler flags, optimizations
- **Development**: Latest code, debugging support

**Pros**: Full control, latest features, cross-platform
**Cons**: Longer setup, requires build tools, more complex

## 💻 Platform-Specific Guides

### Linux
- **[Ubuntu/Debian](linux.md#ubuntu-debian)** - `.deb` packages and apt installation
- **[CentOS/RHEL](linux.md#centos-rhel)** - `.rpm` packages and yum/dnf installation
- **[Alpine Linux](linux.md#alpine-linux)** - `.tar.gz` packages and apk installation
- **[Generic Linux](linux.md#generic-linux)** - Source compilation for any distribution

### macOS
- **[Homebrew](macos.md#homebrew)** - Package manager installation
- **[DMG Installer](macos.md#dmg-installer)** - Graphical installer
- **[PKG Installer](macos.md#pkg-installer)** - Command-line installer
- **[Source Build](macos.md#source-build)** - Build from source code

### Windows
- **[Windows Installer](windows.md#windows-installer)** - MSI installer
- **[Chocolatey](windows.md#chocolatey)** - Package manager installation
- **[WSL](windows.md#wsl)** - Windows Subsystem for Linux
- **[Source Build](windows.md#source-build)** - Build from source code

## 📦 Package Comparison

| Platform | Package Type | Size | Installation | Updates |
|----------|--------------|------|--------------|---------|
| **Linux** | `.deb` | ~15MB | `sudo dpkg -i` | `apt update && apt upgrade` |
| **Linux** | `.rpm` | ~15MB | `sudo rpm -i` | `yum update` or `dnf update` |
| **Linux** | `.tar.gz` | ~15MB | Extract and configure | Manual |
| **macOS** | `.dmg` | ~20MB | Drag and drop | Manual |
| **macOS** | `.pkg` | ~20MB | `sudo installer` | Manual |
| **Windows** | `.msi` | ~25MB | Double-click or `msiexec` | Manual |

## 🔧 Installation Prerequisites

### System Requirements
- **Operating System**: See platform-specific guides
- **Architecture**: x86_64, ARM64, or Universal
- **Memory**: 512MB RAM minimum, 2GB recommended
- **Storage**: 100MB available disk space
- **Network**: Internet access for OAuth2 authentication

### Software Dependencies
- **Python 3.6+** for OAuth2 helper tools
- **Modern web browser** for OAuth2 authentication
- **SSL certificates** for secure email transmission

### Authentication Setup
- **OAuth2 provider** accounts (Gmail, Office 365, etc.)
- **Developer credentials** from provider consoles
- **Domain verification** for production use

## 📋 Installation Checklist

### Before Installation
- [ ] **Choose platform** and installation method
- [ ] **Verify system requirements** are met
- [ ] **Install dependencies** (Python, build tools if needed)
- [ ] **Prepare OAuth2 credentials** for your email provider
- [ ] **Plan configuration** and deployment strategy

### During Installation
- [ ] **Download** appropriate package or source code
- [ ] **Install** ssmtp-mailer system-wide
- [ ] **Verify installation** with basic commands
- [ ] **Set up OAuth2** authentication
- [ ] **Configure basic settings** for your environment

### After Installation
- [ ] **Test email sending** with sample messages
- [ ] **Configure SSL/TLS** for secure transmission
- [ ] **Set up monitoring** and logging
- [ ] **Document configuration** for future reference
- [ ] **Plan maintenance** and update procedures

## 🎯 Choose Your Path

### For Beginners
1. **Use package installation** for your platform
2. **Follow step-by-step** instructions
3. **Use OAuth2 helpers** for authentication
4. **Test with sample emails**

### For System Administrators
1. **Plan deployment** across multiple systems
2. **Use package management** for consistency
3. **Configure monitoring** and alerting
4. **Set up automated updates**

### For Developers
1. **Build from source** for latest features
2. **Customize build** options and flags
3. **Debug and contribute** to the project
4. **Test with development** configurations

### For Enterprise Users
1. **Review security** requirements
2. **Plan OAuth2 deployment** strategy
3. **Configure service accounts** for automation
4. **Set up compliance** and audit logging

## 🔐 Post-Installation Setup

### OAuth2 Configuration
```bash
# List available providers
python3 tools/oauth2-helper/oauth2-helper.py --list

# Set up Gmail OAuth2
python3 tools/oauth2-helper/oauth2-helper.py gmail

# Set up Office 365 OAuth2
python3 tools/oauth2-helper/oauth2-helper.py office365
```

### Basic Configuration
```bash
# Create configuration directory
sudo mkdir -p /etc/ssmtp-mailer
sudo chown $USER:$USER /etc/ssmtp-mailer

# Create basic configuration
cat > /etc/ssmtp-mailer/config.json << 'EOF'
{
  "smtp": {
    "host": "smtp.gmail.com",
    "port": 587,
    "security": "tls",
    "auth": {
      "type": "oauth2",
      "client_id": "YOUR_CLIENT_ID",
      "client_secret": "YOUR_CLIENT_SECRET",
      "refresh_token": "YOUR_REFRESH_TOKEN",
      "user": "your-email@gmail.com"
    }
  }
}
EOF
```

### Testing Installation
```bash
# Test configuration
ssmtp-mailer test --config /etc/ssmtp-mailer/config.json

# Send test email
ssmtp-mailer send \
  --config /etc/ssmtp-mailer/config.json \
  --from "your-email@gmail.com" \
  --to "test@example.com" \
  --subject "Test from ssmtp-mailer" \
  --body "Hello from ssmtp-mailer!"
```

## 🚨 Common Installation Issues

### Package Installation Failures
```bash
# Check dependencies
sudo apt install -f  # Ubuntu/Debian
sudo yum check  # CentOS/RHEL

# Verify package integrity
sha256sum ssmtp-mailer.deb
rpm -K ssmtp-mailer.rpm
```

### Build Failures
```bash
# Check build tools
gcc --version
cmake --version
make --version

# Install missing dependencies
sudo apt install build-essential cmake  # Ubuntu/Debian
sudo yum groupinstall "Development Tools"  # CentOS/RHEL
brew install cmake  # macOS
```

### Permission Issues
```bash
# Fix ownership
sudo chown -R $USER:$USER /etc/ssmtp-mailer
sudo chown -R $USER:$USER /var/log/ssmtp-mailer

# Fix permissions
chmod 600 /etc/ssmtp-mailer/*.json
chmod 644 /var/log/ssmtp-mailer/*.log
```

## 📚 Next Steps

### Configuration
- **[Configuration Overview](../configuration/README.md)** - Understanding configuration options
- **[SSL/TLS Setup](../configuration/ssl-setup.md)** - Secure email transmission
- **[DNS Configuration](../configuration/dns-setup.md)** - Email deliverability

### OAuth2 Setup
- **[OAuth2 Overview](../oauth2/README.md)** - Complete authentication guide
- **[Provider Setup Guides](../oauth2/README.md#email-provider-guides)** - Gmail, Office 365, etc.
- **[Security Best Practices](../oauth2/oauth2-security.md)** - Security guidelines

### Architecture
- **[Architecture Overview](../architecture/README.md)** - System design and components
- **[Relay Setup](../architecture/relay-setup.md)** - Email relay configuration
- **[System Design](../architecture/relay-architecture.md)** - Design principles

## 🆘 Getting Help

### Installation Issues
- **Check prerequisites** in [Prerequisites Guide](../getting-started/prerequisites.md)
- **Review troubleshooting** sections in platform guides
- **Verify system requirements** and dependencies

### Configuration Issues
- **Use OAuth2 helpers** for authentication setup
- **Check configuration** syntax and format
- **Review logs** for error messages

### General Support
- **GitHub Issues**: Report bugs and request features
- **GitHub Discussions**: Ask questions and share solutions
- **Documentation**: Comprehensive guides and examples

---

*Ready to install? Choose your platform from the guides below and get started with ssmtp-mailer!*
