# ssmtp-mailer

**Simple SMTP Mailer** - A smart host MTA (Mail Transfer Agent) for multiple domains with flexible authentication and routing.

## Overview

ssmtp-mailer is a lightweight, high-performance mailer application designed for Linux, macOS, and BSD platforms. It serves as a smart host MTA that can handle multiple domain-specific SMTP relay accounts, making it ideal for web applications that need to send mail from different domains using various service accounts.

## Key Features

- **Multi-Domain Support**: Handle multiple domains with different SMTP relay accounts
- **Flexible Authentication**: Support for OAuth2, username/password, and API key authentication
- **Smart Routing**: Map from-addresses to appropriate SMTP accounts based on domain
- **Template Addresses**: Support for dynamic address patterns like `contact-{type}@domain.com`
- **SSL/TLS Support**: Secure SMTP connections with OpenSSL
- **Configuration Management**: Modular configuration system with `/etc/ssmtp-mailer/conf.d/` support
- **Cross-Platform**: Build for Linux, macOS (Big Sur 11.0+), and BSD platforms
- **Universal Binary Support**: Native support for both Intel and Apple Silicon Macs
- **Package Support**: RPM, DEB, and DMG package generation

## Use Cases

- **Web Applications**: Send confirmation emails, notifications, and alerts
- **Multi-Tenant Systems**: Handle mail for multiple domains from a single server
- **Service Accounts**: Use Gmail, Office 365, or other SMTP providers as relay accounts
- **Distribution Lists**: Send to multiple recipients while maintaining proper from-addresses
- **Development/Testing**: Local mail testing without setting up full mail infrastructure
- **macOS Development**: Build and test mail functionality on macOS systems

## Architecture

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   Application   │───▶│  ssmtp-mailer    │───▶│  SMTP Server    │
│   (Web App)     │    │  (Smart Host)    │    │  (Gmail, etc.)  │
└─────────────────┘    └──────────────────┘    └─────────────────┘
                              │
                              ▼
                       ┌──────────────────┐
                       │  Configuration   │
                       │  Management      │
                       └──────────────────┘
```

## Installation

### Prerequisites

#### Linux
- C++17 compatible compiler (GCC 7+, Clang 5+)
- CMake 3.16+
- OpenSSL development libraries
- pkg-config

#### macOS (Big Sur 11.0+)
- Xcode Command Line Tools (includes Clang and CMake)
- Homebrew (recommended for OpenSSL)
- OpenSSL 3.x (via Homebrew: `brew install openssl@3`)
- CMake 3.16+ (via Homebrew: `brew install cmake`)

### From Source

```bash
# Clone the repository
git clone https://github.com/yourusername/ssmtp-mailer.git
cd ssmtp-mailer

# Install dependencies
make deps

# Build and install
make install
```

### Platform-Specific Builds

#### macOS
```bash
# Build universal binary (Intel + Apple Silicon)
make build-universal

# Build for Intel Macs only
make build-intel

# Build for Apple Silicon Macs only
make build-arm64

# Create macOS DMG package
make package-dmg
```

#### Linux
```bash
# Build 32-bit version
make build-32

# Build 64-bit version
make build-64

# Build both architectures
make build-multiarch
```

### Package Installation

#### RPM (Red Hat/CentOS/Fedora)
```bash
sudo rpm -i ssmtp-mailer-0.1.0-1.x86_64.rpm
```

#### DEB (Debian/Ubuntu)
```bash
sudo dpkg -i ssmtp-mailer_0.1.0_amd64.deb
```

#### DMG (macOS)
```bash
# Mount the DMG and drag the application to Applications folder
# Or install via command line:
sudo installer -pkg /Volumes/ssmtp-mailer-0.1.0/ssmtp-mailer.pkg -target /
```

## Configuration

### Main Configuration

The main configuration file location depends on your platform:

#### Linux
```ini
# /etc/ssmtp-mailer/ssmtp-mailer.conf
[global]
default_hostname = localhost
default_from = root@localhost
config_dir = /etc/ssmtp-mailer/conf.d
domains_dir = /etc/ssmtp-mailer/domains
log_level = info
log_file = /var/log/ssmtp-mailer.log
```

#### macOS
```ini
# /usr/local/etc/ssmtp-mailer/ssmtp-mailer.conf
[global]
default_hostname = localhost
default_from = root@localhost
config_dir = /usr/local/etc/ssmtp-mailer/conf.d
domains_dir = /usr/local/etc/ssmtp-mailer/domains
log_level = info
log_file = /usr/local/var/log/ssmtp-mailer.log
```

### Domain Configuration

Domain-specific settings in the appropriate `conf.d` directory:

```ini
[domain:example.com]
enabled = true
smtp_server = smtp.gmail.com:587
auth_method = oauth2
service_account = noreply@example.com
relay_account = mail-relay@example.com

[domain:another-domain.com]
enabled = true
smtp_server = smtp.office365.com:587
auth_method = username_password
service_account = notifications@another-domain.com
relay_account = mail-service@another-domain.com
```

### User Configuration

User account definitions:

```ini
[user:contact-legal@domain1.com]
domain = domain1.com
enabled = true
can_send_from = true
can_send_to = true
allowed_recipients = ["*@domain1.com", "customers@*"]

[user:contact-{type}@dreamlikenetworks.com]
domain = dreamlikenetworks.com
enabled = true
can_send_from = true
can_send_to = true
template = true
allowed_types = ["legal", "privacy", "general", "support", "sales"]
allowed_recipients = ["*@dreamlikenetworks.com", "partners@*"]
```

### Address Mapping

Routing rules in the `mappings/` directory:

```ini
[route:contact-legal@domain1.com]
from_addresses = ["contact-legal@domain1.com"]
to_domains = ["domain1.com", "customers.*"]
smtp_account = mailer@postal.domain1.com
```

## Usage

### Command Line

```bash
# Send a simple email
ssmtp-mailer --from "contact-support@domain1.com" \
             --to "customer@example.com" \
             --subject "Support Request" \
             --body "Thank you for contacting support."

# Send with HTML content
ssmtp-mailer --from "contact-legal@dreamlikenetworks.com" \
             --to "partner@company.com" \
             --subject "Legal Notice" \
             --html-body "<h1>Legal Notice</h1><p>Important information...</p>"

# Send to multiple recipients
ssmtp-mailer --from "noreply@domain1.com" \
             --to "user1@example.com,user2@example.com" \
             --subject "Newsletter" \
             --body "Monthly newsletter content..."
```

### Programmatic Usage

```cpp
#include <ssmtp-mailer/mailer.hpp>

int main() {
    ssmtp_mailer::Mailer mailer;
    
    ssmtp_mailer::Email email;
    email.from = "contact-support@domain1.com";
    email.to = {"customer@example.com"};
    email.subject = "Support Request";
    email.body = "Thank you for contacting support.";
    
    mailer.send(email);
    
    return 0;
}
```

## Building

### Basic Build

```bash
make build
```

### Platform-Specific Builds

#### macOS
```bash
# Universal binary (Intel + Apple Silicon)
make build-universal

# Intel Macs only
make build-intel

# Apple Silicon Macs only
make build-arm64
```

#### Linux
```bash
# 32-bit version
make build-32

# 64-bit version
make build-64

# Both architectures
make build-multiarch
```

### Package Building

```bash
# Build platform-specific packages
make package

# On macOS: creates DMG package
# On Linux: creates RPM and DEB packages
```

### Development

```bash
# Debug build
make debug

# Release build
make release

# Static analysis
make analyze

# Code formatting
make format

# Style checking
make check-style
```

## Directory Structure

### Linux
```
/etc/ssmtp-mailer/
├── ssmtp-mailer.conf            # Main configuration
├── conf.d/                      # Modular configurations
│   ├── 01-defaults.conf        # Global defaults
│   ├── 10-domains.conf         # Domain definitions
│   ├── 20-users.conf           # User accounts
│   ├── 30-address-mapping.conf # Routing rules
│   ├── 40-smtp-servers.conf    # SMTP servers
│   └── 50-logging.conf         # Logging config
├── domains/                     # Domain-specific configs
├── users/                       # User account definitions
├── mappings/                    # Address routing rules
└── ssl/                         # SSL certificates
```

### macOS
```
/usr/local/etc/ssmtp-mailer/
├── ssmtp-mailer.conf            # Main configuration
├── conf.d/                      # Modular configurations
│   ├── 01-defaults.conf        # Global defaults
│   ├── 10-domains.conf         # Domain definitions
│   ├── 20-users.conf           # User accounts
│   ├── 30-address-mapping.conf # Routing rules
│   ├── 40-smtp-servers.conf    # SMTP servers
│   └── 50-logging.conf         # Logging config
├── domains/                     # Domain-specific configs
├── users/                       # User account definitions
├── mappings/                    # Address routing rules
└── ssl/                         # SSL certificates
```

## Logging

Log file locations depend on your platform:

- **Linux**: `/var/log/ssmtp-mailer.log`
- **macOS**: `/usr/local/var/log/ssmtp-mailer.log`

Log levels include:

- `debug`: Detailed debugging information
- `info`: General information and status
- `warning`: Warning messages
- `error`: Error messages
- `critical`: Critical errors

## Security

- SSL/TLS encryption for all SMTP connections
- Authentication credentials stored securely
- Input validation and sanitization
- Rate limiting for SMTP connections
- Audit logging for security events
- Platform-specific security frameworks (Security.framework on macOS)

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests for new functionality
5. Ensure all tests pass
6. Submit a pull request

## Testing

```bash
# Run all tests
make test

# Run specific test suite
cd build && ctest -R smtp_tests
```

## Troubleshooting

### Common Issues

1. **Authentication Failed**: Check SMTP server credentials and authentication method
2. **Connection Refused**: Verify SMTP server hostname and port
3. **SSL Errors**: Ensure SSL certificates are valid and up-to-date
4. **Permission Denied**: Check file permissions for configuration and log files

### Platform-Specific Issues

#### macOS
- **Xcode Command Line Tools**: Ensure they are installed: `xcode-select --install`
- **OpenSSL**: Use Homebrew version: `brew install openssl@3`
- **Architecture Mismatch**: Use `make build-universal` for compatibility with both Intel and Apple Silicon

#### Linux
- **Library Dependencies**: Install development packages: `sudo apt-get install libssl-dev`
- **Compiler Version**: Ensure GCC 7+ or Clang 5+ is installed

### Debug Mode

```bash
# Enable debug logging
make debug
# Edit the appropriate logging config file for your platform
# Set log_level = debug
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Support

- **Documentation**: [docs/](docs/)
- **Issues**: [GitHub Issues](https://github.com/yourusername/ssmtp-mailer/issues)
- **Discussions**: [GitHub Discussions](https://github.com/yourusername/ssmtp-mailer/discussions)

## Roadmap

- [ ] Daemon mode for background operation
- [ ] Mail queue management
- [ ] DKIM signing support
- [ ] SPF record generation
- [ ] Web-based configuration interface
- [ ] Metrics and monitoring
- [ ] Docker containerization
- [ ] Kubernetes deployment support
- [x] macOS Big Sur+ support
- [x] Universal binary support (Intel + Apple Silicon)

## Acknowledgments

- Inspired by the simplicity of `ssmtp`
- Built with modern C++ standards
- OpenSSL for secure communications
- CMake for cross-platform builds
- Apple frameworks for macOS integration
