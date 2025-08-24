# Linux Build Guide

## Overview

This guide covers building the ssmtp-mailer application on Linux systems. The application is designed to work on most modern Linux distributions and can be built using either CMake or the provided build scripts.

## Prerequisites

### Required Dependencies

#### Core Build Tools
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install build-essential cmake git

# CentOS/RHEL/Rocky Linux
sudo yum groupinstall "Development Tools"
sudo yum install cmake git
# OR for newer versions:
sudo dnf groupinstall "Development Tools"
sudo dnf install cmake git

# Fedora
sudo dnf groupinstall "Development Tools"
sudo dnf install cmake git

# Arch Linux
sudo pacman -S base-devel cmake git

# Alpine Linux
apk add build-base cmake git
```

#### Development Libraries
```bash
# Ubuntu/Debian
sudo apt install libssl-dev libcrypto++-dev

# CentOS/RHEL/Rocky Linux
sudo yum install openssl-devel
# OR for newer versions:
sudo dnf install openssl-devel

# Fedora
sudo dnf install openssl-devel

# Arch Linux
sudo pacman -S openssl

# Alpine Linux
apk add openssl-dev
```

#### Optional Dependencies
```bash
# Ubuntu/Debian
sudo apt install pkg-config libsystemd-dev

# CentOS/RHEL/Rocky Linux
sudo yum install pkgconfig systemd-devel
# OR for newer versions:
sudo dnf install pkgconfig systemd-devel

# Fedora
sudo dnf install pkgconfig systemd-devel

# Arch Linux
sudo pacman -S pkgconf systemd

# Alpine Linux
apk add pkgconfig
```

## Build Methods

### Method 1: Using Build Scripts (Recommended)

The project includes build scripts that handle dependencies and build configuration automatically.

#### Quick Build
```bash
# Clone the repository
git clone https://github.com/your-repo/ssmtp-mailer.git
cd ssmtp-mailer

# Make build script executable
chmod +x scripts/build.sh

# Run the build script
./scripts/build.sh
```

#### Build with Options
```bash
# Build with debug symbols
./scripts/build.sh --debug

# Build with specific CMake options
./scripts/build.sh --cmake-options "-DCMAKE_BUILD_TYPE=Release -DENABLE_TESTS=ON"

# Clean build
./scripts/build.sh --clean

# Install after build
./scripts/build.sh --install
```

### Method 2: Manual CMake Build

For more control over the build process, you can use CMake directly.

#### Basic Build
```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j$(nproc)

# Install (optional)
sudo make install
```

#### Advanced CMake Options
```bash
# Configure with specific options
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/usr/local \
  -DENABLE_TESTS=ON \
  -DENABLE_DOCS=ON \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Build with parallel jobs
make -j$(nproc)

# Run tests (if enabled)
make test

# Install
sudo make install
```

### Method 3: Package Manager Build

Some distributions support building packages directly.

#### Debian/Ubuntu Package
```bash
# Install build dependencies
sudo apt build-dep .

# Build package
dpkg-buildpackage -b -uc -us

# Install the package
sudo dpkg -i ../ssmtp-mailer_*.deb
```

#### RPM Package (CentOS/RHEL/Fedora)
```bash
# Install build dependencies
sudo yum install rpm-build rpmdevtools
# OR for newer versions:
sudo dnf install rpm-build rpmdevtools

# Set up RPM build environment
rpmdev-setuptree

# Build RPM
rpmbuild -ba packaging/rpm/ssmtp-mailer.spec
```

## Distribution-Specific Instructions

### Ubuntu/Debian

#### Ubuntu 20.04 LTS (Focal)
```bash
# Update system
sudo apt update && sudo apt upgrade

# Install dependencies
sudo apt install build-essential cmake git libssl-dev pkg-config

# Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

#### Ubuntu 22.04 LTS (Jammy)
```bash
# Install dependencies
sudo apt install build-essential cmake git libssl-dev pkg-config libsystemd-dev

# Build with systemd support
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_SYSTEMD=ON
make -j$(nproc)
```

### CentOS/RHEL/Rocky Linux

#### CentOS 7
```bash
# Install EPEL repository
sudo yum install epel-release

# Install dependencies
sudo yum groupinstall "Development Tools"
sudo yum install cmake git openssl-devel

# Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

#### CentOS 8/Rocky Linux 8
```bash
# Install dependencies
sudo dnf groupinstall "Development Tools"
sudo dnf install cmake git openssl-devel

# Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

#### Rocky Linux 9
```bash
# Install dependencies
sudo dnf groupinstall "Development Tools"
sudo dnf install cmake git openssl-devel pkgconfig

# Build with newer features
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_CXX17=ON
make -j$(nproc)
```

### Fedora

#### Fedora 36+
```bash
# Install dependencies
sudo dnf groupinstall "Development Tools"
sudo dnf install cmake git openssl-devel pkgconfig systemd-devel

# Build with systemd support
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_SYSTEMD=ON
make -j$(nproc)
```

### Arch Linux

#### Arch Linux (Rolling Release)
```bash
# Install dependencies
sudo pacman -S base-devel cmake git openssl pkgconf

# Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Alpine Linux

#### Alpine Linux 3.16+
```bash
# Install dependencies
apk add build-base cmake git openssl-dev pkgconfig

# Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## Build Configuration Options

### CMake Variables

| Variable | Default | Description |
|----------|---------|-------------|
| `CMAKE_BUILD_TYPE` | `Debug` | Build type (Debug, Release, RelWithDebInfo, MinSizeRel) |
| `CMAKE_INSTALL_PREFIX` | `/usr/local` | Installation prefix |
| `ENABLE_TESTS` | `OFF` | Enable test suite |
| `ENABLE_DOCS` | `OFF` | Build documentation |
| `ENABLE_SYSTEMD` | `OFF` | Enable systemd integration |
| `ENABLE_CXX17` | `ON` | Enable C++17 features |
| `BUILD_SHARED_LIBS` | `OFF` | Build shared libraries |

### Example Configurations

#### Production Build
```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/opt/ssmtp-mailer \
  -DENABLE_TESTS=OFF \
  -DENABLE_DOCS=OFF
```

#### Development Build
```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Debug \
  -DENABLE_TESTS=ON \
  -DENABLE_DOCS=ON \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

#### Custom Installation
```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/home/user/ssmtp-mailer \
  -DCMAKE_INSTALL_SYSCONFDIR=/home/user/ssmtp-mailer/config \
  -DCMAKE_INSTALL_LOCALSTATEDIR=/home/user/ssmtp-mailer/var
```

## Build Output

### Executables
- `ssmtp-mailer` - Main application binary
- `ssmtp-mailer-test` - Test suite (if enabled)

### Libraries
- `libssmtp-mailer.a` - Static library
- `libssmtp-mailer.so` - Shared library (if enabled)

### Configuration Files
- Configuration examples in `config/` directory
- Systemd service files in `packaging/systemd/`

## Installation

### System-Wide Installation
```bash
# Install to system directories
sudo make install

# Default installation paths:
# - Binary: /usr/local/bin/ssmtp-mailer
# - Config: /usr/local/etc/ssmtp-mailer/
# - Libraries: /usr/local/lib/
# - Headers: /usr/local/include/ssmtp-mailer/
```

### User Installation
```bash
# Install to user directory
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/.local
make install

# Add to PATH
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc
```

### Custom Installation
```bash
# Install to custom location
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/ssmtp-mailer
make install

# Create symlinks
sudo ln -s /opt/ssmtp-mailer/bin/ssmtp-mailer /usr/local/bin/
```

## Post-Installation

### Create Configuration Directory
```bash
# Create configuration directory
sudo mkdir -p /etc/ssmtp-mailer/{domains,users,mappings}

# Set permissions
sudo chown -R $USER:$USER /etc/ssmtp-mailer
```

### Systemd Service (Optional)
```bash
# Copy systemd service file
sudo cp packaging/systemd/ssmtp-mailer.service /etc/systemd/system/

# Enable and start service
sudo systemctl enable ssmtp-mailer
sudo systemctl start ssmtp-mailer

# Check status
sudo systemctl status ssmtp-mailer
```

### Verify Installation
```bash
# Check version
ssmtp-mailer --version

# Test configuration
ssmtp-mailer config

# Test SMTP connection
ssmtp-mailer test
```

## Troubleshooting

### Common Build Issues

#### 1. Missing Dependencies
```bash
# Error: Could not find OpenSSL
sudo apt install libssl-dev  # Ubuntu/Debian
sudo yum install openssl-devel  # CentOS/RHEL
```

#### 2. CMake Version Too Old
```bash
# Check CMake version
cmake --version

# Install newer version if needed
# Ubuntu/Debian:
sudo apt install cmake3
# OR download from cmake.org
```

#### 3. Compiler Version Issues
```bash
# Check GCC version
gcc --version

# Minimum required: GCC 7.0 or Clang 5.0
# Install newer version if needed
sudo apt install gcc-9 g++-9  # Ubuntu/Debian
sudo yum install gcc-toolset-9  # CentOS/RHEL
```

#### 4. Permission Issues
```bash
# Fix ownership
sudo chown -R $USER:$USER .

# Fix permissions
chmod +x scripts/build.sh
```

### Build Debugging

#### Verbose Build
```bash
# Verbose make output
make VERBOSE=1

# Verbose CMake output
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_VERBOSE_MAKEFILE=ON
```

#### CMake Debug
```bash
# Show CMake configuration
cmake .. -L

# Show all variables
cmake .. -LAH

# Generate compile commands for tools
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

#### Dependency Check
```bash
# Check installed packages
pkg-config --list-all | grep -i ssl

# Check library paths
ldconfig -p | grep ssl

# Check header files
find /usr/include -name "openssl" -type d
```

## Performance Optimization

### Build Optimization
```bash
# Use all CPU cores
make -j$(nproc)

# Optimize for your CPU
cmake .. -DCMAKE_BUILD_TYPE=Release -march=native

# Enable link-time optimization
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON
```

### Runtime Optimization
```bash
# Profile-guided optimization
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-fprofile-generate"
make
# Run tests to generate profile data
make test
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-fprofile-use -fprofile-correction"
make clean && make
```

## Security Considerations

### Build Security
```bash
# Enable security flags
cmake .. -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_FLAGS="-fstack-protector-strong -D_FORTIFY_SOURCE=2" \
  -DCMAKE_EXE_LINKER_FLAGS="-Wl,-z,relro,-z,now"
```

### Runtime Security
```bash
# Run as non-root user
sudo useradd -r -s /bin/false ssmtp-mailer
sudo chown -R ssmtp-mailer:ssmtp-mailer /etc/ssmtp-mailer

# Set proper file permissions
sudo chmod 755 /etc/ssmtp-mailer
sudo chmod 600 /etc/ssmtp-mailer/*.conf
```

## Maintenance

### Regular Updates
```bash
# Update source code
git pull origin main

# Clean build directory
rm -rf build

# Rebuild
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Dependency Updates
```bash
# Update system packages
sudo apt update && sudo apt upgrade  # Ubuntu/Debian
sudo yum update  # CentOS/RHEL
sudo dnf update  # Fedora
sudo pacman -Syu  # Arch Linux
```

### Cleanup
```bash
# Remove build artifacts
make clean

# Remove build directory
rm -rf build

# Uninstall (if installed)
sudo make uninstall
```

## Support

### Getting Help
1. Check the troubleshooting section above
2. Review build logs for specific error messages
3. Verify your system meets the prerequisites
4. Check for distribution-specific issues

### Useful Commands
```bash
# System information
uname -a
cat /etc/os-release
gcc --version
cmake --version

# Build information
make help
cmake --help
```

### Reporting Issues
When reporting build issues, include:
- Distribution and version
- GCC/Clang version
- CMake version
- Complete error messages
- Build configuration used
