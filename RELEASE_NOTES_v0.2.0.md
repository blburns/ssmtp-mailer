# ssmtp-mailer v0.2.0 Release Notes

## 🎉 What's New in v0.2.0

This is a **major release** that transforms ssmtp-mailer from a basic SMTP tool into a **professional, cross-platform package distribution system** with enterprise-grade features.

## ✨ Major Features

### 🏗️ **Complete Package System**
- **FHS Compliance**: All packages now follow proper Linux filesystem hierarchy standards
- **Cross-Platform Support**: Native packages for Linux, macOS, and Windows
- **Professional Installers**: DMG, PKG, MSI, DEB, RPM, and TGZ packages
- **Smart Architecture Detection**: macOS automatically detects Intel vs Apple Silicon

### 🔐 **OAuth2 Authentication Suite**
- **Multiple Providers**: Gmail, Office 365, SendGrid, Amazon SES, ProtonMail, Zoho Mail, Mailgun, Fastmail
- **Python Scripts**: Automated OAuth2 token generation for each provider
- **Web Interface**: User-friendly web-based OAuth2 helper
- **CLI Launcher**: Unified command-line interface for all OAuth2 tools

### 📦 **Advanced Package Management**
- **Organized Release Structure**: All packages organized by platform and version
- **Automated Build System**: GitHub Actions workflows for continuous integration
- **Release Management**: Professional release coordination across platforms
- **Package Integrity**: SHA256 and MD5 checksums for security

### 📚 **Comprehensive Documentation**
- **Website-like Structure**: Organized documentation with logical navigation
- **Installation Guides**: Platform-specific installation instructions
- **OAuth2 Setup Guides**: Detailed authentication setup for each provider
- **API Reference**: Complete developer documentation
- **Troubleshooting**: Common issues and solutions

## 🚀 New Commands & Tools

### **Release Management**
```bash
make release              # Create complete release with all packages
make package-all          # Build organized packages for all platforms
./scripts/release-manager.sh  # Advanced cross-platform coordination
```

### **OAuth2 Helpers**
```bash
./tools/oauth2-helper/oauth2-helper.py --list  # Show all providers
./tools/oauth2-helper/oauth2-helper.py gmail   # Gmail OAuth2 setup
./tools/oauth2-helper/oauth2-helper.py office365  # Office 365 setup
```

### **GitHub Integration**
```bash
./scripts/upload-to-github.sh  # Upload packages to GitHub releases
./scripts/upload-to-github.sh --dry-run  # Preview what will be uploaded
```

## 🐧 Linux Packages

### **Debian/Ubuntu (.deb)**
- **Dependencies**: libssl3, libcurl4, libjsoncpp25
- **Installation**: `sudo dpkg -i ssmtp-mailer-0.2.0-*.deb`
- **Features**: System integration, automatic dependency resolution

### **Red Hat/CentOS/Fedora (.rpm)**
- **Dependencies**: openssl, libcurl, jsoncpp
- **Installation**: `sudo rpm -i ssmtp-mailer-0.2.0-*.rpm`
- **Features**: Enterprise package management, SELinux support

### **Generic (.tar.gz)**
- **Installation**: Extract and run from any location
- **Features**: Portable, no system dependencies

## 🍎 macOS Packages

### **DMG Installer**
- **Architecture**: Universal (Intel + Apple Silicon) or auto-detected
- **Features**: Drag-and-drop installation, professional appearance
- **Requirements**: macOS 12.0+ (Monterey)

### **PKG Installer**
- **Installation**: Native macOS package installer
- **Features**: System integration, automatic updates
- **Permissions**: Proper file permissions and ownership

## 🪟 Windows Packages

### **NSIS Installer (.exe)**
- **Installation**: Professional Windows installer
- **Features**: Start Menu shortcuts, Desktop shortcuts, PATH integration
- **Requirements**: Windows 10/11, Windows Server 2016+

## 🔧 Technical Improvements

### **Build System**
- **CMake Integration**: Modern CMake-based build system
- **Cross-Platform**: Consistent builds across all platforms
- **Dependency Management**: Automatic library detection and linking
- **Optimization**: Release builds with full optimization

### **Code Quality**
- **C++17 Standard**: Modern C++ features and best practices
- **Error Handling**: Comprehensive error handling and logging
- **Memory Management**: RAII and smart pointer usage
- **Testing**: Automated test suite with CI integration

### **Security**
- **SSL/TLS Support**: Modern encryption standards
- **OAuth2 Security**: Secure authentication flows
- **Package Verification**: Checksums for package integrity
- **Secure Defaults**: Security-focused configuration

## 📁 File Structure

```
ssmtp-mailer-0.2.0/
├── bin/                    # Executables
├── lib/                    # Libraries
├── include/                # Header files
├── etc/ssmtp-mailer/       # Configuration files
├── share/ssmtp-mailer/     # Documentation and tools
│   ├── docs/              # Complete documentation
│   ├── oauth2-helper/     # OAuth2 authentication tools
│   └── examples/          # Usage examples
└── var/log/ssmtp-mailer/  # Log files
```

## 🚀 Getting Started

### **Quick Installation**
```bash
# Linux (Debian/Ubuntu)
sudo dpkg -i ssmtp-mailer-0.2.0-*.deb

# macOS
# Double-click DMG file and drag to Applications

# Windows
# Run the .exe installer
```

### **OAuth2 Setup**
```bash
# Gmail setup
./tools/oauth2-helper/oauth2-helper.py gmail

# Office 365 setup
./tools/oauth2-helper/oauth2-helper.py office365
```

### **Configuration**
```bash
# Edit configuration
sudo nano /etc/ssmtp-mailer/config.conf

# Start service
sudo systemctl start ssmtp-mailer
```

## 🔄 Migration from Previous Versions

### **Breaking Changes**
- **File Locations**: Packages now install to standard system locations
- **Configuration**: New configuration file structure
- **Dependencies**: Updated library requirements

### **Upgrade Path**
1. **Backup** your current configuration
2. **Uninstall** previous version
3. **Install** v0.2.0 packages
4. **Migrate** configuration files
5. **Test** functionality

## 🐛 Bug Fixes

- **Fixed CPack configuration** for Linux package generation
- **Resolved architecture detection** issues on macOS
- **Corrected file permissions** and ownership
- **Fixed dependency resolution** in package managers
- **Resolved compilation warnings** and errors

## 🔮 What's Next

### **Planned Features**
- **Docker Support**: Containerized deployment options
- **Kubernetes Integration**: Cloud-native deployment
- **Monitoring**: Built-in metrics and health checks
- **API**: REST API for programmatic access
- **Plugins**: Extensible plugin system

### **Community Contributions**
- **Documentation**: Help improve guides and examples
- **Testing**: Test on different platforms and configurations
- **Bug Reports**: Report issues and feature requests
- **Code**: Submit pull requests for improvements

## 📊 System Requirements

### **Minimum Requirements**
- **Linux**: Ubuntu 20.04+, CentOS 7+, or equivalent
- **macOS**: macOS 12.0+ (Monterey)
- **Windows**: Windows 10+ or Windows Server 2016+
- **Memory**: 512MB RAM
- **Storage**: 100MB free space

### **Recommended Requirements**
- **Linux**: Ubuntu 22.04+ or CentOS 8+
- **macOS**: macOS 13.0+ (Ventura)
- **Windows**: Windows 11+ or Windows Server 2022+
- **Memory**: 1GB RAM
- **Storage**: 500MB free space

## 🙏 Acknowledgments

Special thanks to the open source community for the excellent tools and libraries that made this release possible:
- **CMake**: Cross-platform build system
- **OpenSSL**: SSL/TLS cryptography
- **libcurl**: HTTP client library
- **jsoncpp**: JSON parsing library
- **GitHub Actions**: Continuous integration platform

## 📞 Support & Community

- **Documentation**: [docs/](docs/) directory
- **Issues**: [GitHub Issues](https://github.com/blburns/ssmtp-mailer/issues)
- **Discussions**: [GitHub Discussions](https://github.com/blburns/ssmtp-mailer/discussions)
- **Email**: support@blburns.com

## 📄 License

This project is licensed under the **Apache License, Version 2.0** - see the [LICENSE](LICENSE) file for details.

---

**🎉 Download ssmtp-mailer v0.2.0 now and experience the future of SMTP mailer packages!**

*Built with ❤️ and ☕ by the ssmtp-mailer team*
