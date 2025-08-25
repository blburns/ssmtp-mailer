# Building ssmtp-mailer on macOS

This guide covers building and installing ssmtp-mailer on macOS Big Sur (11.0) and later versions, including support for both Intel and Apple Silicon Macs.

## System Requirements

- **macOS Version**: Big Sur (11.0) or later
- **Architecture**: Intel (x86_64) or Apple Silicon (arm64)
- **RAM**: 4GB minimum, 8GB recommended
- **Disk Space**: 2GB for build, 1GB for installation

## Prerequisites

### 1. Xcode Command Line Tools

The Xcode Command Line Tools provide the essential build tools including Clang, Make, and CMake.

```bash
# Install Xcode Command Line Tools
xcode-select --install

# Verify installation
xcode-select -p
# Should output: /Library/Developer/CommandLineTools
```

### 2. Homebrew (Recommended)

Homebrew provides easy access to additional dependencies and is the recommended package manager for macOS.

```bash
# Install Homebrew
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Add Homebrew to PATH (if not already done)
if [[ "$(uname -m)" == "arm64" ]]; then
    eval "$(/opt/homebrew/bin/brew shellenv)"
else
    eval "$(/usr/local/bin/brew shellenv)"
fi

# Verify installation
brew --version
```

### 3. Required Dependencies

Install the required build dependencies via Homebrew:

```bash
# Install core dependencies
brew install cmake openssl@3 pkg-config

# Verify installations
cmake --version
pkg-config --version
openssl version
```

## Building from Source

### Quick Build

The easiest way to build ssmtp-mailer on macOS is using the provided Makefile:

```bash
# Clone the repository
git clone https://github.com/yourusername/ssmtp-mailer.git
cd ssmtp-mailer

# Install dependencies (if not already installed)
make deps

# Build universal binary (Intel + Apple Silicon)
make build-universal

# Or build for specific architecture
make build-intel      # Intel Macs only
make build-arm64      # Apple Silicon Macs only
```

### Using the macOS Build Script

For more control over the build process, use the dedicated macOS build script:

```bash
# Make the script executable
chmod +x scripts/build-macos.sh

# Build with default settings (universal binary)
./scripts/build-macos.sh

# Build with specific options
./scripts/build-macos.sh -d                    # Debug build
./scripts/build-macos.sh -a intel              # Intel only
./scripts/build-macos.sh -a arm64              # Apple Silicon only
./scripts/build-macos.sh --package             # Build and create DMG package
./scripts/build-macos.sh --help                # Show all options
```

### Using the Makefile Integration

The Makefile now includes integrated support for the build scripts:

```bash
# Auto-detect platform and use appropriate build script
make build-script

# Force macOS build script
make build-macos

# Build and package using scripts
make package-script
```

### Manual CMake Build

For advanced users who want full control over the build process:

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 \
      -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" \
      ..

# Build
make -j$(sysctl -n hw.ncpu)

# Run tests
make test

# Install
sudo make install
```

## Architecture Options

### Universal Binary (Recommended)

Universal binaries run natively on both Intel and Apple Silicon Macs:

```bash
# Build universal binary
make build-universal

# Or with CMake
cmake -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" ..
```

### Intel Macs Only

Optimized for Intel-based Macs:

```bash
# Build for Intel only
make build-intel

# Or with CMake
cmake -DCMAKE_OSX_ARCHITECTURES="x86_64" ..
```

### Apple Silicon Only

Optimized for Apple Silicon Macs:

```bash
# Build for Apple Silicon only
make build-arm64

# Or with CMake
cmake -DCMAKE_OSX_ARCHITECTURES="arm64" ..
```

## Package Generation

### DMG Package

Create a macOS disk image for easy distribution:

```bash
# Build and create DMG package
make package-dmg

# Or with the build script
./scripts/build-macos.sh --package
```

The DMG package will be created in the `dist/` directory.

### Installer Package

Create a macOS installer package:

```bash
# Configure CMake with installer support
cmake -DBUILD_MACOS_INSTALLER=ON ..

# Build and package
make package
```

## Installation

### System Installation

Install to the system-wide location:

```bash
# Install
make install

# Uninstall
make uninstall
```

**Note**: On macOS, the default installation path is `/usr/local/` instead of `/usr/`.

### Configuration Files

After installation, configuration files are located at:

```
/usr/local/etc/ssmtp-mailer/
├── ssmtp-mailer.conf
├── conf.d/
├── domains/
├── users/
├── mappings/
└── ssl/
```

### Log Files

Log files are written to:

```
/usr/local/var/log/ssmtp-mailer.log
```

## Troubleshooting

### Common Issues

#### 1. Xcode Command Line Tools Not Found

```bash
# Error: xcode-select: error: tool 'xcodebuild' requires Xcode
# Solution: Install Xcode Command Line Tools
xcode-select --install
```

#### 2. OpenSSL Not Found

```bash
# Error: Could NOT find OpenSSL
# Solution: Install via Homebrew
brew install openssl@3

# Set OpenSSL paths if needed
export OPENSSL_ROOT_DIR=$(brew --prefix openssl@3)
export OPENSSL_LIBRARIES=$(brew --prefix openssl@3)/lib
```

#### 3. Architecture Mismatch

```bash
# Error: Bad CPU type in executable
# Solution: Build universal binary or for your specific architecture
make build-universal
```

#### 4. Permission Denied

```bash
# Error: Permission denied
# Solution: Use sudo for system-wide installation
sudo make install
```

### Build Verification

Verify your build was successful:

```bash
# Check executable architecture
file build/bin/ssmtp-mailer

# Expected output for universal binary:
# build/bin/ssmtp-mailer: Mach-O universal binary with 2 architectures
# build/bin/ssmtp-mailer (for architecture x86_64): Mach-O 64-bit executable x86_64
# build/bin/ssmtp-mailer (for architecture arm64): Mach-O 64-bit executable arm64

# Check library architecture
file build/lib/libssmtp-mailer.dylib

# Run basic functionality test
./build/bin/ssmtp-mailer --help
```

### Performance Optimization

For optimal performance on your Mac:

```bash
# Use all available CPU cores
make -j$(sysctl -n hw.ncpu)

# Enable optimizations
cmake -DCMAKE_BUILD_TYPE=Release ..

# Profile-guided optimization (advanced)
cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_PGO=ON ..
```

## Development

### Debug Builds

```bash
# Debug build with symbols
make debug

# Or with CMake
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

### Code Analysis

```bash
# Static analysis with clang-tidy
make analyze

# Code formatting
make format

# Style checking
make check-style
```

### Testing

```bash
# Run all tests
make test

# Run specific test suite
cd build && ctest -R smtp_tests

# Run tests with verbose output
cd build && ctest --verbose
```

## Integration with macOS

### Keychain Integration

ssmtp-mailer can integrate with macOS Keychain for secure credential storage:

```bash
# Enable Keychain integration
cmake -DENABLE_MACOS_KEYCHAIN=ON ..
```

### Security Framework

Leverage macOS Security framework for enhanced security:

```bash
# Enable Security framework features
cmake -DENABLE_MACOS_SECURITY=ON ..
```

### Launch Services

Register with macOS Launch Services for system integration:

```bash
# Install and register
sudo make install
sudo launchctl load /usr/local/etc/ssmtp-mailer/ssmtp-mailer.plist
```

## Distribution

### Creating Distribution Packages

```bash
# Create DMG for distribution
make package-dmg

# Create installer package
cmake -DBUILD_MACOS_INSTALLER=ON ..
make package
```

### Code Signing

For distribution outside your organization, consider code signing:

```bash
# Sign the executable (requires Apple Developer account)
codesign --force --sign "Developer ID Application: Your Name" build/bin/ssmtp-mailer

# Sign the library
codesign --force --sign "Developer ID Application: Your Name" build/lib/libssmtp-mailer.dylib

# Verify signatures
codesign --verify build/bin/ssmtp-mailer
codesign --verify build/lib/libssmtp-mailer.dylib
```

### Notarization

For macOS Catalina and later, consider notarizing your application:

```bash
# Notarize the application (requires Apple Developer account)
xcrun altool --notarize-app --primary-bundle-id "com.example.ssmtp-mailer" \
    --username "your-apple-id@example.com" \
    --password "app-specific-password" \
    --file dist/ssmtp-mailer-0.2.0.dmg
```

## Support

For additional help with macOS builds:

- Check the [main README](../README.md) for general information
- Review [GitHub Issues](https://github.com/yourusername/ssmtp-mailer/issues) for known problems
- Join [GitHub Discussions](https://github.com/yourusername/ssmtp-mailer/discussions) for community support

## Version Compatibility

| macOS Version | Minimum Required | Recommended | Notes |
|---------------|------------------|-------------|-------|
| Big Sur (11.0) | ✅ | ✅ | Full support |
| Monterey (12.0) | ✅ | ✅ | Full support |
| Ventura (13.0) | ✅ | ✅ | Full support |
| Sonoma (14.0) | ✅ | ✅ | Full support |
| Sequoia (15.0) | ✅ | ✅ | Full support |

**Note**: While ssmtp-mailer may work on earlier macOS versions, only Big Sur 11.0+ is officially supported and tested.
