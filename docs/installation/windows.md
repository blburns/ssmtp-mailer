# Windows Installation Guide

This guide covers installing ssmtp-mailer on Windows systems using various methods and supporting Windows 10, Windows 11, and Windows Server.

## 🚀 Quick Installation

### Windows Installer (Recommended)
```bash
# Download latest MSI installer
curl -L -o ssmtp-mailer.msi "https://github.com/blburns/ssmtp-mailer/releases/latest/download/ssmtp-mailer_windows_x64.msi"

# Install using msiexec
msiexec /i ssmtp-mailer.msi /quiet

# Or double-click the MSI file in Windows Explorer
# Then follow the installation wizard
```

### Chocolatey Installation
```bash
# Install Chocolatey (if not already installed)
# Run PowerShell as Administrator and execute:
Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))

# Install ssmtp-mailer
choco install ssmtp-mailer

# Verify installation
ssmtp-mailer --version
```

### WSL (Windows Subsystem for Linux)
```bash
# Install WSL (Windows 10/11)
wsl --install

# Restart Windows and open WSL
wsl

# Follow Linux installation guide
# See: ../installation/linux.md
```

## 📦 Package Installation

### Windows Installer (.msi)

#### Download and Install
```bash
# Download latest release
curl -L -o ssmtp-mailer.msi "https://github.com/blburns/ssmtp-mailer/releases/latest/download/ssmtp-mailer_windows_x64.msi"

# Install using command line
msiexec /i ssmtp-mailer.msi /quiet

# Or install with logging
msiexec /i ssmtp-mailer.msi /quiet /l*v install.log
```

#### Verify Installation
```bash
# Check version
ssmtp-mailer --version

# Check binary location
where ssmtp-mailer

# Check Windows Programs and Features
# Control Panel > Programs > Programs and Features
```

#### Update Package
```bash
# Uninstall old version
msiexec /x ssmtp-mailer.msi /quiet

# Install new version
msiexec /i ssmtp-mailer.msi /quiet
```

### Chocolatey Installation

#### Install via Chocolatey
```bash
# Open PowerShell as Administrator

# Install Chocolatey (if not already installed)
Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))

# Install ssmtp-mailer
choco install ssmtp-mailer

# Verify installation
ssmtp-mailer --version
```

#### Verify Installation
```bash
# Check version
ssmtp-mailer --version

# Check binary location
where ssmtp-mailer

# Check Chocolatey info
choco info ssmtp-mailer
```

#### Update via Chocolatey
```bash
# Update Chocolatey
choco upgrade all

# Upgrade ssmtp-mailer
choco upgrade ssmtp-mailer
```

### WSL Installation

#### Install WSL
```bash
# Enable WSL feature
dism.exe /online /enable-feature /featurename:Microsoft-Windows-Subsystem-Linux /all /norestart

# Enable Virtual Machine feature
dism.exe /online /enable-feature /featurename:VirtualMachinePlatform /all /norestart

# Restart Windows
shutdown /r /t 0
```

#### Install Linux Distribution
```bash
# Install Ubuntu (recommended)
wsl --install -d Ubuntu

# Or install other distributions
wsl --list --online
wsl --install -d Debian
```

#### Install ssmtp-mailer in WSL
```bash
# Open WSL terminal
wsl

# Follow Linux installation guide
# See: ../installation/linux.md
```

## 🛠️ Build from Source

### Prerequisites

#### System Requirements
- **Windows Version**: Windows 10 (1809+) or Windows 11
- **Architecture**: x64 (64-bit)
- **RAM**: 4GB minimum, 8GB recommended
- **Disk Space**: 2GB for build, 1GB for installation

#### Visual Studio Build Tools
```bash
# Install Visual Studio Build Tools
# Download from: https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022

# Or install via winget
winget install Microsoft.VisualStudio.2022.BuildTools

# Include these workloads:
# - MSVC v143 - VS 2022 C++ x64/x86 build tools
# - Windows 10/11 SDK
# - CMake tools for Visual Studio
```

#### CMake
```bash
# Install CMake
winget install Kitware.CMake

# Or download from: https://cmake.org/download/

# Verify installation
cmake --version
```

#### Git
```bash
# Install Git
winget install Git.Git

# Or download from: https://git-scm.com/download/win

# Verify installation
git --version
```

### Build Methods

#### Method 1: Using Visual Studio
```bash
# Clone the repository
git clone https://github.com/blburns/ssmtp-mailer.git
cd ssmtp-mailer

# Open Developer Command Prompt for VS 2022
# Start > Visual Studio 2022 > Developer Command Prompt for VS 2022

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -G "Visual Studio 17 2022" -A x64

# Build the project
cmake --build . --config Release
```

#### Method 2: Using CMake Command Line
```bash
# Clone the repository
git clone https://github.com/blburns/ssmtp-mailer.git
cd ssmtp-mailer

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

# Build the project
cmake --build . --config Release
```

#### Method 3: Using WSL
```bash
# Install WSL and follow Linux build guide
# See: ../installation/linux.md
```

### Build Configuration Options

#### CMake Generators
```bash
# Visual Studio 2022 (recommended)
-G "Visual Studio 17 2022" -A x64

# MinGW Makefiles
-G "MinGW Makefiles"

# Ninja (if installed)
-G "Ninja"
```

#### Build Options
```bash
# Build type
-DCMAKE_BUILD_TYPE=Release    # Release build (default)
-DCMAKE_BUILD_TYPE=Debug      # Debug build with symbols
-DCMAKE_BUILD_TYPE=RelWithDebInfo  # Release with debug info

# Architecture
-DCMAKE_SYSTEM_PROCESSOR=x64  # Target x64

# Features
-DENABLE_TESTS=ON             # Enable test suite
-DENABLE_DOCS=ON              # Build documentation
-DUSE_SYSTEM_LIBS=ON          # Use system libraries
```

#### Compiler Options
```bash
# Set compiler flags
set CMAKE_C_FLAGS="/O2 /MT"
set CMAKE_CXX_FLAGS="/O2 /MT"

# Configure with custom flags
cmake .. -DCMAKE_C_FLAGS="%CMAKE_C_FLAGS%" -DCMAKE_CXX_FLAGS="%CMAKE_CXX_FLAGS%"
```

## 🔧 Post-Installation Setup

### Create Required Directories
```bash
# Create configuration and log directories
mkdir "C:\ProgramData\ssmtp-mailer"
mkdir "C:\ProgramData\ssmtp-mailer\logs"
mkdir "C:\ProgramData\ssmtp-mailer\spool"

# Set permissions (run as Administrator)
icacls "C:\ProgramData\ssmtp-mailer" /grant Users:F /T
```

### Install OAuth2 Helper Tools
```bash
# Install Python (if not already installed)
winget install Python.Python.3.11

# Install Python dependencies
pip install requests

# Verify OAuth2 helper
python tools/oauth2-helper/oauth2-helper.py --list

# Set up Gmail OAuth2
python tools/oauth2-helper/oauth2-helper.py gmail
```

### Basic Configuration
```bash
# Create configuration file
echo { > "C:\ProgramData\ssmtp-mailer\config.json"
echo   "smtp": { >> "C:\ProgramData\ssmtp-mailer\config.json"
echo     "host": "smtp.gmail.com", >> "C:\ProgramData\ssmtp-mailer\config.json"
echo     "port": 587, >> "C:\ProgramData\ssmtp-mailer\config.json"
echo     "security": "tls", >> "C:\ProgramData\ssmtp-mailer\config.json"
echo     "auth": { >> "C:\ProgramData\ssmtp-mailer\config.json"
echo       "type": "oauth2", >> "C:\ProgramData\ssmtp-mailer\config.json"
echo       "client_id": "YOUR_CLIENT_ID", >> "C:\ProgramData\ssmtp-mailer\config.json"
echo       "client_secret": "YOUR_CLIENT_SECRET", >> "C:\ProgramData\ssmtp-mailer\config.json"
echo       "refresh_token": "YOUR_REFRESH_TOKEN", >> "C:\ProgramData\ssmtp-mailer\config.json"
echo       "user": "your-email@gmail.com" >> "C:\ProgramData\ssmtp-mailer\config.json"
echo     } >> "C:\ProgramData\ssmtp-mailer\config.json"
echo   } >> "C:\ProgramData\ssmtp-mailer\config.json"
echo } >> "C:\ProgramData\ssmtp-mailer\config.json"
```

## 🧪 Testing Installation

### Basic Tests
```bash
# Test binary
ssmtp-mailer --version
ssmtp-mailer --help

# Test configuration
ssmtp-mailer test --config "C:\ProgramData\ssmtp-mailer\config.json"

# Test SMTP connection
ssmtp-mailer test --config "C:\ProgramData\ssmtp-mailer\config.json" --smtp
```

### Email Tests
```bash
# Send test email
ssmtp-mailer send ^
  --config "C:\ProgramData\ssmtp-mailer\config.json" ^
  --from "your-email@gmail.com" ^
  --to "test@example.com" ^
  --subject "Test from ssmtp-mailer" ^
  --body "Hello from ssmtp-mailer!"

# Test with HTML
ssmtp-mailer send ^
  --config "C:\ProgramData\ssmtp-mailer\config.json" ^
  --from "your-email@gmail.com" ^
  --to "test@example.com" ^
  --subject "HTML Test" ^
  --body "<h1>Hello World</h1>" ^
  --html
```

## 🚨 Troubleshooting

### Common Issues

#### "Command not found: ssmtp-mailer"
```bash
# Check if binary exists
dir "C:\Program Files\ssmtp-mailer\ssmtp-mailer.exe"

# Add to PATH
setx PATH "%PATH%;C:\Program Files\ssmtp-mailer"

# Restart command prompt
```

#### "Permission denied"
```bash
# Run Command Prompt as Administrator
# Right-click Command Prompt > Run as administrator

# Check file permissions
icacls "C:\ProgramData\ssmtp-mailer\config.json"
```

#### "Build failed"
```bash
# Check Visual Studio Build Tools
# Install or repair via Visual Studio Installer

# Check CMake version
cmake --version  # Should be 3.16+

# Clean build directory
rmdir /s build
mkdir build
cd build
```

#### "OAuth2 authentication failed"
```bash
# Check Python installation
python --version

# Install Python dependencies
pip install requests

# Verify OAuth2 helper
python tools/oauth2-helper/oauth2-helper.py --list

# Regenerate tokens
python tools/oauth2-helper/oauth2-helper.py gmail
```

#### "DLL not found"
```bash
# Install Visual C++ Redistributable
# Download from: https://aka.ms/vs/17/release/vc_redist.x64.exe

# Or install via winget
winget install Microsoft.VCRedist.2015+.x64
```

### Debug Commands
```bash
# Check system information
ver
systeminfo

# Check installed programs
wmic product get name,version

# Check file locations
where ssmtp-mailer
dir "C:\Program Files\ssmtp-mailer"

# Check configuration
type "C:\ProgramData\ssmtp-mailer\config.json"
```

## 📚 Next Steps

### Configuration
- **[Configuration Overview](../configuration/README.md)** - Understanding configuration options
- **[SSL/TLS Setup](../configuration/ssl-setup.md)** - Secure email transmission
- **[DNS Configuration](../configuration/dns-setup.md)** - Email deliverability

### OAuth2 Setup
- **[OAuth2 Overview](../oauth2/README.md)** - Complete authentication guide
- **[Gmail Setup](../oauth2/gmail-oauth2-setup.md)** - Gmail OAuth2 configuration
- **[Office 365 Setup](../oauth2/office365-oauth2-setup.md)** - Office 365 OAuth2 configuration

### Architecture
- **[Architecture Overview](../architecture/README.md)** - System design and components
- **[Relay Setup](../architecture/relay-setup.md)** - Email relay configuration

## 🆘 Getting Help

### Installation Issues
- **Check prerequisites** in [Prerequisites Guide](../getting-started/prerequisites.md)
- **Verify system requirements** and dependencies
- **Review troubleshooting** sections above

### Configuration Issues
- **Use OAuth2 helpers** for authentication setup
- **Check configuration** syntax and format
- **Review logs** for error messages

### General Support
- **GitHub Issues**: Report bugs and request features
- **GitHub Discussions**: Ask questions and share solutions
- **Documentation**: Comprehensive guides and examples

---

*ssmtp-mailer is now installed on your Windows system! Next, set up OAuth2 authentication and configure your email relay system.*
