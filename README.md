# ssmtp-mailer

**Simple SMTP Mailer** - A smart host MTA (Mail Transfer Agent) for multiple domains with flexible authentication and routing.

## Overview

ssmtp-mailer is a lightweight, high-performance mailer application designed for Linux and BSD platforms. It serves as a smart host MTA that can handle multiple domain-specific SMTP relay accounts, making it ideal for web applications that need to send mail from different domains using various service accounts.

## Key Features

- **Multi-Domain Support**: Handle multiple domains with different SMTP relay accounts
- **Flexible Authentication**: Support for OAuth2, username/password, and API key authentication
- **Smart Routing**: Map from-addresses to appropriate SMTP accounts based on domain
- **Template Addresses**: Support for dynamic address patterns like `contact-{type}@domain.com`
- **SSL/TLS Support**: Secure SMTP connections with OpenSSL
- **Configuration Management**: Modular configuration system with `/etc/ssmtp-mailer/conf.d/` support
- **Cross-Platform**: Build for both 32-bit and 64-bit architectures
- **Package Support**: RPM and DEB package generation

## Use Cases

- **Web Applications**: Send confirmation emails, notifications, and alerts
- **Multi-Tenant Systems**: Handle mail for multiple domains from a single server
- **Service Accounts**: Use Gmail, Office 365, or other SMTP providers as relay accounts
- **Distribution Lists**: Send to multiple recipients while maintaining proper from-addresses
- **Development/Testing**: Local mail testing without setting up full mail infrastructure

## Architecture

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   Application  │───▶│  ssmtp-mailer   │───▶│  SMTP Server   │
│   (Web App)    │    │   (Smart Host)   │    │  (Gmail, etc.) │
└─────────────────┘    └──────────────────┘    └─────────────────┘
                              │
                              ▼
                       ┌──────────────────┐
                       │  Configuration   │
                       │  Management     │
                       └──────────────────┘
```

## Installation

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+)
- CMake 3.16+
- OpenSSL development libraries
- pkg-config

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

### Package Installation

#### RPM (Red Hat/CentOS/Fedora)
```bash
sudo rpm -i ssmtp-mailer-1.0.0-1.x86_64.rpm
```

#### DEB (Debian/Ubuntu)
```bash
sudo dpkg -i ssmtp-mailer_1.0.0_amd64.deb
```

## Configuration

### Main Configuration

The main configuration file is located at `/etc/ssmtp-mailer/ssmtp-mailer.conf`:

```ini
[global]
default_hostname = localhost
default_from = root@localhost
config_dir = /etc/ssmtp-mailer/conf.d
domains_dir = /etc/ssmtp-mailer/domains
log_level = info
log_file = /var/log/ssmtp-mailer.log
```

### Domain Configuration

Domain-specific settings in `/etc/ssmtp-mailer/conf.d/10-domains.conf`:

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

User account definitions in `/etc/ssmtp-mailer/conf.d/20-users.conf`:

```ini
[user:contact-legal@dreamlikelabs.com]
domain = dreamlikelabs.com
enabled = true
can_send_from = true
can_send_to = true
allowed_recipients = ["*@dreamlikelabs.com", "customers@*"]

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

Routing rules in `/etc/ssmtp-mailer/mappings/`:

```ini
[route:contact-legal@dreamlikelabs.com]
from_addresses = ["contact-legal@dreamlikelabs.com"]
to_domains = ["dreamlikelabs.com", "customers.*"]
smtp_account = mailer@postal.dreamlikelabs.com
```

## Usage

### Command Line

```bash
# Send a simple email
ssmtp-mailer --from "contact-support@dreamlikelabs.com" \
             --to "customer@example.com" \
             --subject "Support Request" \
             --body "Thank you for contacting support."

# Send with HTML content
ssmtp-mailer --from "contact-legal@dreamlikenetworks.com" \
             --to "partner@company.com" \
             --subject "Legal Notice" \
             --html-body "<h1>Legal Notice</h1><p>Important information...</p>"

# Send to multiple recipients
ssmtp-mailer --from "noreply@blburns.com" \
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
    email.from = "contact-support@dreamlikelabs.com";
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

### Multi-Architecture Build

```bash
# Build 32-bit version
make build-32

# Build 64-bit version
make build-64

# Build both
make build-multiarch
```

### Package Building

```bash
# Build RPM package
make package-rpm

# Build DEB package
make package-deb

# Build both
make package
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

## Logging

Logs are written to `/var/log/ssmtp-mailer.log` by default. Log levels include:

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

### Debug Mode

```bash
# Enable debug logging
make debug
# Edit /etc/ssmtp-mailer/conf.d/50-logging.conf
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

## Acknowledgments

- Inspired by the simplicity of `ssmtp`
- Built with modern C++ standards
- OpenSSL for secure communications
- CMake for cross-platform builds
