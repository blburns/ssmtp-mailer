# 🚀 simple-smtp-mailer

**High-performance SMTP mailer with queue management and OAuth2 support**

[![License: Apache 2.0](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![Platform: macOS](https://img.shields.io/badge/Platform-macOS-blue.svg)](https://www.apple.com/macos/)
[![Platform: Linux](https://img.shields.io/badge/Platform-Linux-green.svg)](https://www.linux.org/)
[![Language: C++](https://img.shields.io/badge/Language-C++-orange.svg)](https://isocpp.org/)

## 📋 Table of Contents

- [Features](#-features)
- [Quick Start](#-quick-start)
- [Installation](#-installation)
- [Configuration](#-configuration)
- [Usage](#-usage)
- [Development](#-development)
- [Documentation](#-documentation)
- [Contributing](#-contributing)
- [License](#-license)

## ✨ Features

### 🚀 **High Performance**
- **Optimized C++ Implementation**: Built for speed and efficiency
- **Parallel Processing**: Multi-threaded email queue processing
- **Memory Efficient**: Smart memory management and resource handling

### 📧 **Advanced SMTP Support**
- **Full SMTP Protocol**: Complete RFC compliance
- **Multiple Encryption**: STARTTLS, SSL, and unencrypted connections
- **Authentication Methods**: PLAIN, LOGIN, CRAM-MD5, and OAuth2
- **Connection Pooling**: Efficient connection reuse and management

### 🔐 **OAuth2 Integration**
- **Google Service Accounts**: Native Gmail API support
- **Microsoft Graph**: Office 365 and Outlook integration
- **Secure Token Management**: Automatic token refresh and storage
- **Multiple Providers**: Extensible authentication framework

### 📊 **Queue Management**
- **Persistent Storage**: Reliable email queuing with disk persistence
- **Priority Handling**: Configurable email priority levels
- **Retry Logic**: Intelligent retry with exponential backoff
- **Dead Letter Queue**: Failed email handling and analysis

### 🛡️ **Security & Reliability**
- **SSL/TLS Support**: Modern encryption standards
- **Certificate Validation**: Proper SSL certificate verification
- **Input Sanitization**: Protection against injection attacks
- **Logging & Monitoring**: Comprehensive audit trails

## 🚀 Quick Start

### Prerequisites
- **macOS**: Big Sur 11.0+ or later
- **Linux**: Ubuntu 20.04+, CentOS 8+, or compatible distributions
- **C++ Compiler**: GCC 9+ or Clang 12+
- **CMake**: 3.16 or later

### Quick Installation

#### macOS
```bash
# Clone the repository
git clone https://github.com/blburns/simple-smtp-mailer.git
cd simple-smtp-mailer

# Install dependencies
make deps

# Build with auto-detected architecture
make build-script

# Create installer package
make package-dmg

# Install (double-click the DMG or use installer)
sudo installer -pkg dist/simple-smtp-mailer-*.pkg -target /
```

#### Linux
```bash
# Clone the repository
git clone https://github.com/blburns/simple-smtp-mailer.git
cd simple-smtp-mailer

# Install dependencies
make deps

# Build
make build

# Create platform-specific package
make package

# Install
sudo make install
```

### First Run
```bash
# Test installation
simple-smtp-mailer --version

# View help
simple-smtp-mailer --help

# Check configuration
simple-smtp-mailer --config-check
```

## 📦 Installation

### Package Types

#### macOS
- **DMG Package**: Drag-and-drop installation with GUI
- **PKG Installer**: Native macOS installer with post-install scripts
- **Universal Binary**: Intel + Apple Silicon compatibility

#### Linux
- **DEB Package**: Debian/Ubuntu family distributions
- **RPM Package**: Red Hat/CentOS/Fedora distributions
- **Generic TGZ**: Arch, Alpine, Gentoo, and other distributions

### Installation Methods

#### Automated Installation
```bash
# macOS - Smart auto-detection
make build-script && make package-dmg

# Linux - Smart OS detection
make build && make package
```

#### Manual Installation
```bash
# Build from source
make clean && make build

# Install to system
sudo make install

# Create custom package
make package-custom
```

### Installation Paths
```
/usr/local/bin/simple-smtp-mailer          # Main executable
/usr/local/lib/libsimple-smtp-mailer.dylib # Shared library
/usr/local/include/simple-smtp-mailer/     # Header files
/etc/simple-smtp-mailer/                   # Configuration files
/var/log/simple-smtp-mailer/               # Log files
```

## ⚙️ Configuration

### Main Configuration File
```ini
# /etc/simple-smtp-mailer/simple-smtp-mailer.conf

[General]
log_level = INFO
log_file = /var/log/simple-smtp-mailer/simple-smtp-mailer.log
max_log_size = 10
max_log_files = 5

[SMTP]
default_server = smtp.gmail.com
default_port = 587
default_encryption = STARTTLS
connection_timeout = 30
auth_timeout = 10

[Queue]
max_queue_size = 1000
retry_delay = 60
max_retries = 3
process_interval = 5

[Security]
verify_ssl = true
min_tls_version = 1.2
```

### OAuth2 Configuration
```ini
# /etc/simple-smtp-mailer/auth/auth.conf

[Service_Accounts]
google_service_account_file = /path/to/service-account.json
google_scopes = https://www.googleapis.com/auth/gmail.send

microsoft_tenant_id = your-tenant-id
microsoft_client_id = your-client-id
microsoft_client_secret = your-client-secret
```

### Email Templates
```ini
# /etc/simple-smtp-mailer/templates/email.conf

[Templates]
welcome_subject = Welcome to our service
welcome_body = Thank you for joining our service!
notification_subject = Important notification
notification_body = You have an important notification.
```

## 💻 Usage

### Command Line Interface

#### Basic Usage
```bash
# Send a simple email
simple-smtp-mailer send --to user@example.com --subject "Hello" --body "Message body"

# Send with attachments
simple-smtp-mailer send --to user@example.com --subject "Report" --body "See attached" --attach report.pdf

# Send to multiple recipients
simple-smtp-mailer send --to user1@example.com,user2@example.com --subject "Group message" --body "Hello everyone"
```

#### Queue Management
```bash
# List queued emails
simple-smtp-mailer queue list

# Show failed emails
simple-smtp-mailer queue failed

# Retry failed emails
simple-smtp-mailer queue retry

# Clear queue
simple-smtp-mailer queue clear
```

#### Configuration Management
```bash
# Validate configuration
simple-smtp-mailer config validate

# Test SMTP connection
simple-smtp-mailer config test-connection

# Show current settings
simple-smtp-mailer config show
```

### Programmatic Usage

#### C++ API
```cpp
#include <simple-smtp-mailer/email.hpp>
#include <simple-smtp-mailer/mailer.hpp>

// Create email
    ssmtp_mailer::Email email;
email.setFrom("sender@example.com");
email.addTo("recipient@example.com");
email.setSubject("Test Email");
email.setBody("Hello from simple-smtp-mailer!");

// Send email
ssmtp_mailer::Mailer mailer;
    mailer.send(email);
```

#### Python Bindings (Future)
```python
import ssmtp_mailer

# Create and send email
email = ssmtp_mailer.Email(
    from_addr="sender@example.com",
    to_addrs=["recipient@example.com"],
    subject="Python Test",
    body="Hello from Python!"
)

mailer = ssmtp_mailer.Mailer()
mailer.send(email)
```

## 🛠️ Development

### Building from Source

#### Prerequisites
```bash
# macOS
xcode-select --install
brew install cmake openssl jsoncpp curl

# Ubuntu/Debian
sudo apt update
sudo apt install build-essential cmake libssl-dev libjsoncpp-dev libcurl4-openssl-dev

# CentOS/RHEL
sudo yum groupinstall "Development Tools"
sudo yum install cmake openssl-devel jsoncpp-devel libcurl-devel
```

#### Build Commands
```bash
# Clean build
make clean

# Debug build
make debug

# Release build
make release

# Build with tests
make build && make test

# Build specific architecture (macOS)
make build-intel      # Intel only
make build-arm64      # Apple Silicon only
make build-universal  # Both architectures
```

#### Development Tools
```bash
# Code formatting
make format

# Style checking
make check-style

# Static analysis
make analyze

# Run tests
make test

# Build documentation
make docs
```

### Project Structure
```
simple-smtp-mailer/
├── src/                    # Source code
│   ├── core/              # Core functionality
│   │   ├── auth/          # Authentication modules
│   │   ├── config/        # Configuration management
│   │   ├── logging/       # Logging system
│   │   ├── queue/         # Email queue management
│   │   └── smtp/          # SMTP client implementation
│   ├── email.cpp          # Email class implementation
│   ├── mailer.cpp         # Main mailer class
│   └── main.cpp           # Command-line interface
├── include/                # Header files
├── scripts/                # Build and installation scripts
├── docs/                   # Documentation
├── tests/                  # Test suite
├── CMakeLists.txt          # CMake configuration
└── Makefile                # Build automation
```

### Testing
```bash
# Run all tests
make test

# Run specific test suite
make test-unit
make test-integration
make test-performance

# Generate coverage report
make coverage
```

## 📚 Documentation

### Available Documentation
- **Installation Guide**: `docs/installation/`
- **Configuration Reference**: `docs/configuration/`
- **API Documentation**: `docs/api/`
- **Development Guide**: `docs/development/`

### Building Documentation
```bash
# Generate API docs
make docs-api

# Build user manual
make docs-manual

# Create PDF documentation
make docs-pdf
```

## 🤝 Contributing

We welcome contributions! Here's how you can help:

### Getting Started
1. **Fork** the repository
2. **Clone** your fork locally
3. **Create** a feature branch
4. **Make** your changes
5. **Test** thoroughly
6. **Submit** a pull request

### Development Guidelines
- Follow the existing code style
- Add tests for new features
- Update documentation as needed
- Ensure all tests pass
- Follow commit message conventions

### Code Style
- **C++**: Follow Google C++ Style Guide
- **Naming**: Use descriptive names and consistent conventions
- **Comments**: Document complex logic and public APIs
- **Error Handling**: Use proper error codes and logging

### Testing Requirements
- **Unit Tests**: Required for all new functionality
- **Integration Tests**: For complex features
- **Performance Tests**: For performance-critical code
- **Coverage**: Maintain >80% test coverage

## 📄 License

This project is licensed under the **Apache License, Version 2.0** - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **OpenSSL**: For cryptographic functionality
- **libcurl**: For HTTP/HTTPS support
- **jsoncpp**: For JSON parsing and generation
- **CMake**: For build system management
- **Contributors**: All who have helped improve this project

## 📞 Support

### Getting Help
- **Documentation**: Check the [docs/](docs/) directory
- **Issues**: Report bugs on [GitHub Issues](https://github.com/blburns/simple-smtp-mailer/issues)
- **Discussions**: Join conversations on [GitHub Discussions](https://github.com/blburns/simple-smtp-mailer/discussions)
- **Wiki**: Check the [GitHub Wiki](https://github.com/blburns/simple-smtp-mailer/wiki)

### Community
- **GitHub**: [https://github.com/blburns/simple-smtp-mailer](https://github.com/blburns/simple-smtp-mailer)
- **Issues**: [https://github.com/blburns/simple-smtp-mailer/issues](https://github.com/blburns/simple-smtp-mailer/issues)
- **Releases**: [https://github.com/blburns/simple-smtp-mailer/releases](https://github.com/blburns/simple-smtp-mailer/releases)

---

**Made with ❤️ by the simple-smtp-mailer community**

*If you find this project useful, please consider giving it a ⭐ on GitHub!*
