# Prerequisites

Before installing ssmtp-mailer, ensure your system meets these requirements and has the necessary dependencies installed.

## 🖥️ System Requirements

### Operating Systems
- **Linux**: Ubuntu 20.04+, Debian 11+, CentOS 8+, RHEL 8+, Alpine 3.14+
- **macOS**: macOS 12.0+ (Monterey), macOS 13.0+ (Ventura), macOS 14.0+ (Sonoma)
- **Windows**: Windows 10 (1809+), Windows 11, Windows Server 2019+

### Architecture Support
- **x86_64**: Intel/AMD 64-bit processors
- **ARM64**: Apple Silicon (M1/M2/M3), ARM64 servers
- **Universal**: macOS packages with both architectures

### Hardware Requirements
- **CPU**: 1 GHz processor minimum, 2 GHz recommended
- **Memory**: 512 MB RAM minimum, 2 GB recommended
- **Storage**: 100 MB available disk space minimum
- **Network**: Internet access for OAuth2 authentication

## 🔧 Software Dependencies

### Required Dependencies

#### Python 3.6+
```bash
# Check Python version
python3 --version

# Install Python (Ubuntu/Debian)
sudo apt update
sudo apt install python3 python3-pip

# Install Python (CentOS/RHEL)
sudo yum install python3 python3-pip
# or
sudo dnf install python3 python3-pip

# Install Python (macOS)
brew install python3

# Install Python (Windows)
# Download from https://python.org/downloads/
```

#### Python Packages
```bash
# Install required packages
pip3 install requests

# Or install from requirements file
pip3 install -r tools/oauth2-helper/requirements.txt

# Verify installation
python3 -c "import requests; print('requests installed successfully')"
```

### Optional Dependencies

#### Development Tools
```bash
# Build tools (for building from source)
sudo apt install build-essential cmake  # Ubuntu/Debian
sudo yum groupinstall "Development Tools"  # CentOS/RHEL
brew install cmake  # macOS

# Git (for source code)
sudo apt install git  # Ubuntu/Debian
sudo yum install git  # CentOS/RHEL
brew install git  # macOS
```

#### SSL/TLS Libraries
```bash
# OpenSSL development libraries
sudo apt install libssl-dev  # Ubuntu/Debian
sudo yum install openssl-devel  # CentOS/RHEL
brew install openssl  # macOS

# CA certificates
sudo apt install ca-certificates  # Ubuntu/Debian
sudo yum install ca-certificates  # CentOS/RHEL
# macOS includes CA certificates by default
```

## 🌐 Network Requirements

### Internet Access
- **OAuth2 Authentication**: Required for Gmail, Office 365, and other providers
- **Package Downloads**: For installation and updates
- **SSL Certificate Validation**: For secure email transmission

### Firewall Configuration
```bash
# Required ports
# SMTP: 25, 587, 465 (outbound)
# HTTP: 80, 443 (outbound for OAuth2)
# Local callback: 8080 (for OAuth2 helpers)

# Example UFW rules (Ubuntu)
sudo ufw allow out 25/tcp
sudo ufw allow out 587/tcp
sudo ufw allow out 465/tcp
sudo ufw allow out 80/tcp
sudo ufw allow out 443/tcp
sudo ufw allow in 8080/tcp  # Only for localhost
```

### Proxy Configuration
```bash
# Set proxy environment variables if needed
export http_proxy="http://proxy.company.com:8080"
export https_proxy="http://proxy.company.com:8080"
export HTTP_PROXY="http://proxy.company.com:8080"
export HTTPS_PROXY="http://proxy.company.com:8080"

# For Python requests
export REQUESTS_CA_BUNDLE="/path/to/ca-bundle.crt"
```

## 🔐 Authentication Requirements

### OAuth2 Setup
- **Google Cloud Console** account (for Gmail)
- **Azure Portal** account (for Office 365)
- **Provider-specific** developer accounts
- **Domain verification** for production use

### Service Accounts (Enterprise)
- **Google Workspace Admin** access (for domain-wide delegation)
- **Azure AD Global Administrator** or **Application Administrator** access
- **Service principal** creation permissions

## 📁 File System Requirements

### Directory Permissions
```bash
# Create required directories
sudo mkdir -p /etc/ssmtp-mailer
sudo mkdir -p /var/log/ssmtp-mailer
sudo mkdir -p /var/spool/ssmtp-mailer

# Set ownership
sudo chown $USER:$USER /etc/ssmtp-mailer
sudo chown $USER:$USER /var/log/ssmtp-mailer
sudo chown $USER:$USER /var/spool/ssmtp-mailer

# Set permissions
sudo chmod 755 /etc/ssmtp-mailer
sudo chmod 755 /var/log/ssmtp-mailer
sudo chmod 755 /var/spool/ssmtp-mailer
```

### File Permissions
```bash
# Configuration files
chmod 600 /etc/ssmtp-mailer/config.json
chmod 600 /etc/ssmtp-mailer/oauth2_tokens.json

# Log files
chmod 644 /var/log/ssmtp-mailer/*.log

# OAuth2 token files
chmod 600 oauth2_tokens.json
chmod 600 office365_oauth2_tokens.json
```

## 🧪 Verification Commands

### System Check
```bash
# Check operating system
uname -a
cat /etc/os-release  # Linux
sw_vers  # macOS
ver  # Windows

# Check architecture
uname -m
arch

# Check available memory
free -h  # Linux
vm_stat  # macOS
wmic computersystem get TotalPhysicalMemory  # Windows

# Check disk space
df -h
```

### Dependency Check
```bash
# Check Python
python3 --version
pip3 --version

# Check required packages
python3 -c "import requests; print('✓ requests installed')"
python3 -c "import urllib3; print('✓ urllib3 installed')"

# Check build tools
gcc --version
cmake --version
make --version
```

### Network Check
```bash
# Check internet connectivity
ping -c 3 google.com
curl -I https://google.com

# Check SMTP connectivity
telnet smtp.gmail.com 587
telnet smtp.office365.com 587

# Check local ports
netstat -tuln | grep :8080
lsof -i :8080
```

## 🚨 Common Issues

### Python Not Found
```bash
# Ubuntu/Debian
sudo apt install python3 python3-pip

# CentOS/RHEL
sudo yum install python3 python3-pip

# macOS
brew install python3

# Windows
# Download from python.org and add to PATH
```

### Permission Denied
```bash
# Fix directory permissions
sudo chown -R $USER:$USER /etc/ssmtp-mailer
sudo chown -R $USER:$USER /var/log/ssmtp-mailer

# Fix file permissions
chmod 600 /etc/ssmtp-mailer/*.json
chmod 644 /var/log/ssmtp-mailer/*.log
```

### Network Connectivity Issues
```bash
# Check firewall
sudo ufw status
sudo iptables -L

# Check proxy settings
echo $http_proxy
echo $https_proxy

# Test with curl
curl -v --proxy $http_proxy https://google.com
```

### SSL/TLS Issues
```bash
# Update CA certificates
sudo update-ca-certificates  # Ubuntu/Debian
sudo yum update ca-certificates  # CentOS/RHEL

# Check OpenSSL
openssl version
openssl s_client -connect smtp.gmail.com:587 -starttls smtp
```

## 📋 Pre-Installation Checklist

### System Requirements
- [ ] **Operating system** is supported
- [ ] **Architecture** is compatible
- [ ] **Hardware** meets minimum requirements
- [ ] **Network access** is available

### Software Dependencies
- [ ] **Python 3.6+** is installed
- [ ] **Required packages** are installed
- [ ] **Build tools** are available (if building from source)
- [ ] **SSL libraries** are installed

### Network Configuration
- [ ] **Internet access** is working
- [ ] **Firewall rules** are configured
- [ ] **Proxy settings** are configured (if needed)
- [ ] **SMTP ports** are accessible

### Authentication Setup
- [ ] **OAuth2 provider** accounts are created
- [ ] **Developer credentials** are obtained
- [ ] **Domain verification** is complete (for production)
- [ ] **Service accounts** are configured (for enterprise)

### File System
- [ ] **Required directories** are created
- [ ] **Permissions** are set correctly
- [ ] **Disk space** is available
- [ ] **User ownership** is configured

## 🔗 Next Steps

Once you've verified all prerequisites are met:

1. **Choose your platform** from [Installation Guides](../installation/README.md)
2. **Follow the installation** instructions for your platform
3. **Set up OAuth2** authentication using [OAuth2 Guides](../oauth2/README.md)
4. **Configure your email relay** following [Configuration Guides](../configuration/README.md)

---

*Having trouble with prerequisites? Check the [troubleshooting guide](../oauth2/oauth2-troubleshooting.md) or [open an issue](https://github.com/blburns/ssmtp-mailer/issues) on GitHub.*
