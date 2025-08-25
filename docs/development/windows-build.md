# Building ssmtp-mailer on Windows

This guide explains how to build and use ssmtp-mailer on Windows systems.

## Prerequisites

### Required Software

1. **Visual Studio 2019 or 2022** (Community Edition is fine)
   - Download from: https://visualstudio.microsoft.com/downloads/
   - Install with "Desktop development with C++" workload
   - Make sure to include MSVC v143 compiler

2. **CMake 3.16 or later**
   - Download from: https://cmake.org/download/
   - Add to PATH during installation

3. **OpenSSL for Windows**
   - Download from: https://slproweb.com/products/Win32OpenSSL.html
   - Install to `C:\OpenSSL-Win64` (default location)
   - Add `C:\OpenSSL-Win64\bin` to your PATH

### Optional Software

- **Git for Windows**: https://git-scm.com/download/win
- **NSIS**: For creating Windows installers (included in CMake)

## Quick Build

### Using Build Scripts

1. **Batch Script** (Command Prompt):
   ```cmd
   scripts\build-windows.bat
   ```

2. **PowerShell Script**:
   ```powershell
   powershell -ExecutionPolicy Bypass -File scripts\build-windows.ps1
   ```

### Manual Build

1. **Open Developer Command Prompt**:
   - Start → Visual Studio 2022 → Developer Command Prompt for VS 2022

2. **Clone and Build**:
   ```cmd
   git clone https://github.com/your-repo/ssmtp-mailer.git
   cd ssmtp-mailer
   mkdir build
   cd build
   
   cmake .. -G "Visual Studio 17 2022" -A x64
   cmake --build . --config Release
   ```

## Build Options

### CMake Configuration Options

- `-DCMAKE_BUILD_TYPE=Release` - Build optimized release version
- `-DCMAKE_BUILD_TYPE=Debug` - Build debug version with symbols
- `-DENABLE_SSL=ON` - Enable SSL/TLS support (requires OpenSSL)
- `-DBUILD_SHARED_LIBS=ON` - Build shared libraries (.dll)
- `-DBUILD_TESTS=OFF` - Disable test building (faster build)
- `-DBUILD_EXAMPLES=OFF` - Disable example building

### Visual Studio Generators

- `"Visual Studio 17 2022"` - Visual Studio 2022
- `"Visual Studio 16 2019"` - Visual Studio 2019
- `"Visual Studio 15 2017"` - Visual Studio 2017

### Architecture Options

- `-A x64` - 64-bit (recommended)
- `-A Win32` - 32-bit
- `-A ARM64` - ARM64 (Windows on ARM)

## Output Files

After successful build, you'll find:

- **Executable**: `build\bin\Release\ssmtp-mailer.exe`
- **Library**: `build\lib\Release\ssmtp-mailer.dll`
- **Import Library**: `build\lib\Release\ssmtp-mailer.lib`

## Creating Windows Installer

To create a Windows installer package:

```cmd
cd build
cmake --build . --target package
```

This will create an NSIS installer in the build directory.

## Configuration

### Default Configuration Paths

- **Config Directory**: `%APPDATA%\ssmtp-mailer\config`
- **Log Directory**: `%APPDATA%\ssmtp-mailer\logs`

### Sample Configuration

Copy the example configuration files:

```cmd
mkdir "%APPDATA%\ssmtp-mailer\config"
copy config\ssmtp-mailer.conf.example "%APPDATA%\ssmtp-mailer\config\ssmtp-mailer.conf"
copy config\domains\example.com.conf.example "%APPDATA%\ssmtp-mailer\config\domains\example.com.conf"
copy config\users\user@example.com.conf.example "%APPDATA%\ssmtp-mailer\config\users\user@example.com.conf"
```

## Usage

### Command Line Interface

```cmd
# Show help
ssmtp-mailer.exe --help

# Test configuration
ssmtp-mailer.exe config

# Test SMTP connection
ssmtp-mailer.exe test

# Send email
ssmtp-mailer.exe send --from user@example.com --to recipient@domain.com --subject "Test" --body "Hello World"
```

### Environment Variables

- `SSMTP_MAILER_CONFIG` - Path to configuration file
- `SSMTP_MAILER_LOG_LEVEL` - Log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)

## Troubleshooting

### Common Issues

1. **"cl.exe not found"**
   - Run from Visual Studio Developer Command Prompt
   - Or add Visual Studio tools to PATH

2. **"OpenSSL not found"**
   - Install OpenSSL to `C:\OpenSSL-Win64`
   - Or set `OPENSSL_ROOT_DIR` environment variable

3. **"CMake not found"**
   - Install CMake and add to PATH
   - Or use full path to cmake.exe

4. **Build errors**
   - Check Visual Studio installation
   - Ensure C++17 support is enabled
   - Check Windows SDK version

### Debug Build

For debugging, use Debug configuration:

```cmd
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Debug
cmake --build . --config Debug
```

### Dependencies

The following libraries are automatically linked:
- `ws2_32` - Windows Sockets
- `iphlpapi` - IP Helper API
- `crypt32` - Cryptography API
- `advapi32` - Advanced Windows 32 API
- `userenv` - User Environment API
- `shell32` - Shell API

## Performance

### Optimization

- Use Release build for production
- Enable link-time optimization: `-DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON`
- Use static linking for single executable: `-DBUILD_SHARED_LIBS=OFF`

### Memory Management

- Windows handles memory differently than Unix systems
- Use Windows-specific memory allocation when needed
- Consider using Windows Heap API for large allocations

## Security

### Windows Security Features

- ASLR (Address Space Layout Randomization) enabled by default
- DEP (Data Execution Prevention) enabled
- Windows Defender integration
- UAC (User Account Control) support

### SSL/TLS

- OpenSSL integration for secure connections
- Windows Certificate Store support
- SNI (Server Name Indication) support

## Support

For Windows-specific issues:

1. Check Windows Event Viewer for system errors
2. Verify Windows SDK version compatibility
3. Ensure Visual Studio redistributables are installed
4. Check Windows Firewall settings for network access

## License

This project is licensed under the same terms as the main project.
