#!/bin/bash

# ssmtp-mailer macOS PKG Builder Script
# This script creates a native macOS PKG installer package

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Script variables
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$(dirname "$SCRIPT_DIR")")"
BUILD_DIR="$PROJECT_ROOT/build"
DIST_DIR="$PROJECT_ROOT/dist"
PKG_BUILD_DIR="$PROJECT_ROOT/pkg-build"
COMPONENT_DIR="$PKG_BUILD_DIR/components"
SCRIPTS_DIR="$PKG_BUILD_DIR/scripts"
RESOURCES_DIR="$PKG_BUILD_DIR/resources"

# Package information
PACKAGE_ID="com.ssmtp.mailer"
PACKAGE_VERSION="0.2.0"
PACKAGE_TITLE="ssmtp-mailer"
PACKAGE_DESCRIPTION="High-performance SMTP mailer with queue management and OAuth2 support"
PACKAGE_VENDOR="ssmtp-mailer Project"
PACKAGE_INSTALL_LOCATION="/usr/local"

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to show usage
show_usage() {
    cat << EOF
Usage: $0 [OPTIONS]

Build a native macOS PKG installer for ssmtp-mailer.

OPTIONS:
    -h, --help              Show this help message
    -v, --version           Show version information
    -b, --build-dir PATH    Build directory (default: $BUILD_DIR)
    -d, --dist-dir PATH     Distribution directory (default: $DIST_DIR)
    -p, --pkg-dir PATH      PKG build directory (default: $PKG_BUILD_DIR)
    -i, --install-location  Installation location (default: $PACKAGE_INSTALL_LOCATION)
    --clean                 Clean PKG build directory before building
    --no-scripts            Skip post-install scripts
    --verbose               Enable verbose output

EXAMPLES:
    $0                        # Build PKG with default settings
    $0 --clean               # Clean build and create PKG
    $0 -p /tmp/pkg-build     # Use custom PKG build directory

EOF
}

# Function to show version
show_version() {
    echo "ssmtp-mailer PKG builder v1.0.0"
}

# Function to check dependencies
check_dependencies() {
    print_status "Checking PKG build dependencies..."
    
    local missing_deps=()
    
    # Check for pkgbuild
    if ! command -v pkgbuild &> /dev/null; then
        missing_deps+=("pkgbuild")
    fi
    
    # Check for productbuild
    if ! command -v productbuild &> /dev/null; then
        missing_deps+=("productbuild")
    fi
    
    # Check for xcrun
    if ! command -v xcrun &> /dev/null; then
        missing_deps+=("xcrun")
    fi
    
    if [ ${#missing_deps[@]} -gt 0 ]; then
        print_error "Missing required tools: ${missing_deps[*]}"
        print_error "Please install Xcode command line tools: xcode-select --install"
        exit 1
    fi
    
    print_success "All PKG build dependencies are available"
}

# Function to clean PKG build directory
clean_pkg_build() {
    if [ "$1" = "--clean" ] || [ "$1" = "true" ]; then
        print_status "Cleaning PKG build directory..."
        rm -rf "$PKG_BUILD_DIR"
        print_success "PKG build directory cleaned"
    fi
}

# Function to create PKG build structure
create_pkg_structure() {
    print_status "Creating PKG build structure..."
    
    # Create main directories
    mkdir -p "$COMPONENT_DIR"
    mkdir -p "$SCRIPTS_DIR"
    mkdir -p "$RESOURCES_DIR"
    
    # Create component subdirectories
    mkdir -p "$COMPONENT_DIR/bin"
    mkdir -p "$COMPONENT_DIR/lib"
    mkdir -p "$COMPONENT_DIR/include"
    mkdir -p "$COMPONENT_DIR/etc"
    mkdir -p "$COMPONENT_DIR/var"
    
    print_success "PKG build structure created"
}

# Function to copy build artifacts
copy_build_artifacts() {
    print_status "Copying build artifacts..."
    
    # Copy binary
    if [ -f "$BUILD_DIR/bin/ssmtp-mailer" ]; then
        cp "$BUILD_DIR/bin/ssmtp-mailer" "$COMPONENT_DIR/bin/"
        chmod +x "$COMPONENT_DIR/bin/ssmtp-mailer"
        print_success "Binary copied to PKG component"
    else
        print_error "Binary not found: $BUILD_DIR/bin/ssmtp-mailer"
        exit 1
    fi
    
    # Copy library if it exists
    if [ -f "$BUILD_DIR/lib/libssmtp-mailer.dylib" ]; then
        cp "$BUILD_DIR/lib/libssmtp-mailer.dylib" "$COMPONENT_DIR/lib/"
        print_success "Library copied to PKG component"
    fi
    
    # Copy headers if they exist
    if [ -d "$BUILD_DIR/include" ]; then
        cp -r "$BUILD_DIR/include"/* "$COMPONENT_DIR/include/"
        print_success "Headers copied to PKG component"
    fi
    
    print_success "Build artifacts copied successfully"
}

# Function to create configuration files
create_config_files() {
    print_status "Creating configuration files..."
    
    # Main configuration file
    cat > "$COMPONENT_DIR/etc/ssmtp-mailer.conf" << 'EOF'
# ssmtp-mailer Configuration File
# This file contains the main configuration for ssmtp-mailer

[General]
# Log level: DEBUG, INFO, WARNING, ERROR, CRITICAL
log_level = INFO

# Log file path
log_file = /var/log/ssmtp-mailer/ssmtp-mailer.log

# Maximum log file size in MB
max_log_size = 10

# Number of log files to keep
max_log_files = 5

[SMTP]
# Default SMTP server
default_server = smtp.gmail.com
default_port = 587

# Default encryption (STARTTLS, SSL, NONE)
default_encryption = STARTTLS

# Connection timeout in seconds
connection_timeout = 30

# Authentication timeout in seconds
auth_timeout = 10

[Queue]
# Maximum number of emails in queue
max_queue_size = 1000

# Retry delay in seconds
retry_delay = 60

# Maximum retry attempts
max_retries = 3

# Queue processing interval in seconds
process_interval = 5

[Security]
# SSL certificate verification
verify_ssl = true

# Allowed cipher suites (empty = system default)
allowed_ciphers = 

# Minimum TLS version
min_tls_version = 1.2
EOF

    # Create configuration subdirectories
    mkdir -p "$COMPONENT_DIR/etc/ssmtp-mailer/templates"
    mkdir -p "$COMPONENT_DIR/etc/ssmtp-mailer/ssl"
    mkdir -p "$COMPONENT_DIR/etc/ssmtp-mailer/auth"
    
    # Email template configuration
    cat > "$COMPONENT_DIR/etc/ssmtp-mailer/templates/email.conf" << 'EOF'
# Email Template Configuration
# Define default email templates and settings

[Default]
# Default sender name
sender_name = ssmtp-mailer

# Default reply-to address
reply_to = 

# Default priority
default_priority = NORMAL

# Default encoding
default_encoding = UTF-8

# Maximum message size in MB
max_message_size = 25

[Templates]
# Welcome email template
welcome_subject = Welcome to our service
welcome_body = Thank you for joining our service!

# Notification email template
notification_subject = Important notification
notification_body = You have an important notification.
EOF

    # SSL configuration
    cat > "$COMPONENT_DIR/etc/ssmtp-mailer/ssl/ssl.conf" << 'EOF'
# SSL Configuration
# SSL certificate and key settings

[Certificates]
# CA certificate bundle path
ca_bundle = /etc/ssl/certs/ca-certificates.crt

# Client certificate path (if using client cert auth)
client_cert = 

# Client private key path
client_key = 

[Security]
# Minimum key size in bits
min_key_size = 2048

# Allowed hash algorithms
allowed_hashes = SHA256, SHA384, SHA512

# Disable weak ciphers
disable_weak_ciphers = true
EOF

    # Authentication configuration
    cat > "$COMPONENT_DIR/etc/ssmtp-mailer/auth/auth.conf" << 'EOF'
# Authentication Configuration
# SMTP authentication settings

[SMTP_Auth]
# Default authentication method
default_method = PLAIN

# Supported authentication methods
supported_methods = PLAIN, LOGIN, CRAM-MD5, XOAUTH2

# OAuth2 configuration
oauth2_client_id = 
oauth2_client_secret = 
oauth2_redirect_uri = 
oauth2_token_url = 

[Service_Accounts]
# Google Service Account settings
google_service_account_file = 
google_scopes = https://www.googleapis.com/auth/gmail.send

# Microsoft Graph settings
microsoft_tenant_id = 
microsoft_client_id = 
microsoft_client_secret = 
EOF

    print_success "Configuration files created successfully"
}

# Function to create post-install script
create_postinstall_script() {
    print_status "Creating post-install script..."
    
    cat > "$SCRIPTS_DIR/postinstall" << 'EOF'
#!/bin/bash

# ssmtp-mailer Post-Install Script
# This script runs after the package is installed

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Installation paths
INSTALL_PREFIX="/usr/local"
CONFIG_DIR="/etc/ssmtp-mailer"
LOG_DIR="/var/log/ssmtp-mailer"

print_status "Running ssmtp-mailer post-install script..."

# Create log directory if it doesn't exist
if [ ! -d "$LOG_DIR" ]; then
    print_status "Creating log directory: $LOG_DIR"
    mkdir -p "$LOG_DIR"
fi

# Set proper permissions
print_status "Setting file permissions..."

# Set ownership for configuration
if [ -d "$CONFIG_DIR" ]; then
    chown -R root:wheel "$CONFIG_DIR"
    chmod 755 "$CONFIG_DIR"
    find "$CONFIG_DIR" -type f -name "*.conf" -exec chmod 644 {} \;
    find "$CONFIG_DIR" -type d -exec chmod 755 {} \;
fi

# Set ownership for logs
if [ -d "$LOG_DIR" ]; then
    chown -R root:wheel "$LOG_DIR"
    chmod 755 "$LOG_DIR"
fi

# Create symlinks for easier access
print_status "Creating symlinks..."

# Binary symlink
if [ ! -L "/usr/bin/ssmtp-mailer" ]; then
    ln -sf "$INSTALL_PREFIX/bin/ssmtp-mailer" "/usr/bin/ssmtp-mailer"
    print_success "Binary symlink created: /usr/bin/ssmtp-mailer"
fi

# Library symlink
if [ -f "$INSTALL_PREFIX/lib/libssmtp-mailer.dylib" ] && [ ! -L "/usr/lib/libssmtp-mailer.dylib" ]; then
    ln -sf "$INSTALL_PREFIX/lib/libssmtp-mailer.dylib" "/usr/lib/libssmtp-mailer.dylib"
    print_success "Library symlink created: /usr/lib/libssmtp-mailer.dylib"
fi

# Update dynamic library cache
print_status "Updating dynamic library cache..."
if command -v ldconfig &> /dev/null; then
    ldconfig 2>/dev/null || true
fi

# Create initial log file
if [ -d "$LOG_DIR" ]; then
    touch "$LOG_DIR/ssmtp-mailer.log"
    chown root:wheel "$LOG_DIR/ssmtp-mailer.log"
    chmod 644 "$LOG_DIR/ssmtp-mailer.log"
    print_success "Initial log file created"
fi

print_success "Post-install script completed successfully"
print_status "ssmtp-mailer is now ready to use!"
print_status "Configuration files are located at: $CONFIG_DIR"
print_status "Logs will be written to: $LOG_DIR"
print_status "Use 'ssmtp-mailer --help' to see available options"

exit 0
EOF

    # Make script executable
    chmod +x "$SCRIPTS_DIR/postinstall"
    
    print_success "Post-install script created successfully"
}

# Function to create pre-uninstall script
create_preuninstall_script() {
    print_status "Creating pre-uninstall script..."
    
    cat > "$SCRIPTS_DIR/preuninstall" << 'EOF'
#!/bin/bash

# ssmtp-mailer Pre-Uninstall Script
# This script runs before the package is uninstalled

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_status "Running ssmtp-mailer pre-uninstall script..."

# Remove symlinks
if [ -L "/usr/bin/ssmtp-mailer" ]; then
    rm -f "/usr/bin/ssmtp-mailer"
    print_success "Binary symlink removed"
fi

if [ -L "/usr/lib/libssmtp-mailer.dylib" ]; then
    rm -f "/usr/lib/libssmtp-mailer.dylib"
    print_success "Library symlink removed"
fi

# Update dynamic library cache
if command -v ldconfig &> /dev/null; then
    ldconfig 2>/dev/null || true
fi

print_success "Pre-uninstall script completed successfully"

exit 0
EOF

    # Make script executable
    chmod +x "$SCRIPTS_DIR/preuninstall"
    
    print_success "Pre-uninstall script created successfully"
}

# Function to create package info
create_package_info() {
    print_status "Creating package information..."
    
    # Create component property list
    cat > "$COMPONENT_DIR/component.plist" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleIdentifier</key>
    <string>$PACKAGE_ID</string>
    <key>CFBundleName</key>
    <string>$PACKAGE_TITLE</string>
    <key>CFBundleVersion</key>
    <string>$PACKAGE_VERSION</string>
    <key>CFBundleShortVersionString</key>
    <string>$PACKAGE_VERSION</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleSignature</key>
    <string>????</string>
    <key>CFBundleExecutable</key>
    <string>ssmtp-mailer</string>
    <key>CFBundleInfoDictionaryVersion</key>
    <string>6.0</string>
    <key>CFBundleDisplayName</key>
    <string>$PACKAGE_TITLE</string>
    <key>CFBundleDescription</key>
    <string>$PACKAGE_DESCRIPTION</string>
    <key>CFBundleVendor</key>
    <string>$PACKAGE_VENDOR</string>
    <key>CFBundleCopyright</key>
    <string>Copyright © 2025 B.L. Burns. All rights reserved.</string>
    <key>CFBundleDocumentTypes</key>
    <array>
        <dict>
            <key>CFBundleTypeName</key>
            <string>ssmtp-mailer Configuration</string>
            <key>CFBundleTypeExtensions</key>
            <array>
                <string>conf</string>
            </array>
            <key>CFBundleTypeRole</key>
            <string>Viewer</string>
        </dict>
    </array>
</dict>
</plist>
EOF

    print_success "Package information created successfully"
}

# Function to create distribution XML
create_distribution_xml() {
    print_status "Creating distribution XML..."
    
    cat > "$PKG_BUILD_DIR/distribution.xml" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<installer-gui-script minSpecVersion="1">
    <title>$PACKAGE_TITLE</title>
    <organization>com.ssmtp.mailer</organization>
    <domains enable_localSystem="true"/>
    <options customize="never" require-scripts="true" rootVolumeOnly="true"/>
    <pkg-ref id="$PACKAGE_ID"/>
    <choices-outline>
        <line choice="$PACKAGE_ID"/>
    </choices-outline>
    <choice id="$PACKAGE_ID" title="$PACKAGE_TITLE">
        <pkg-ref id="$PACKAGE_ID"/>
    </choice>
    <pkg-ref id="$PACKAGE_ID" auth="Root" onConclusion="none">$PACKAGE_TITLE-$PACKAGE_VERSION.pkg</pkg-ref>
    <background mime-type="image/png" file="background.png" scaling="proportional" alignment="left"/>
    <welcome file="welcome.html"/>
    <conclusion file="conclusion.html"/>
    <license file="license.txt"/>
</installer-gui-script>
EOF

    print_success "Distribution XML created successfully"
}

# Function to create welcome HTML
create_welcome_html() {
    print_status "Creating welcome HTML..."
    
    cat > "$RESOURCES_DIR/welcome.html" << 'EOF'
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>Welcome to ssmtp-mailer</title>
    <style>
        body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; margin: 40px; }
        .header { text-align: center; margin-bottom: 30px; }
        .content { max-width: 600px; margin: 0 auto; }
        .feature { margin: 20px 0; padding: 15px; background: #f5f5f5; border-radius: 8px; }
        .feature h3 { margin-top: 0; color: #333; }
        .warning { background: #fff3cd; border: 1px solid #ffeaa7; padding: 15px; border-radius: 8px; margin: 20px 0; }
    </style>
</head>
<body>
    <div class="content">
        <div class="header">
            <h1>🚀 Welcome to ssmtp-mailer</h1>
            <p><strong>High-performance SMTP mailer with queue management and OAuth2 support</strong></p>
        </div>
        
        <p>This installer will set up ssmtp-mailer on your system with the following features:</p>
        
        <div class="feature">
            <h3>📧 Advanced SMTP Support</h3>
            <p>Full SMTP protocol support with STARTTLS, SSL, and authentication methods including OAuth2.</p>
        </div>
        
        <div class="feature">
            <h3>⚡ High Performance</h3>
            <p>Optimized C++ implementation with efficient queue management and parallel processing.</p>
        </div>
        
        <div class="feature">
            <h3>🔐 OAuth2 Integration</h3>
            <p>Built-in support for Google Service Accounts and Microsoft Graph authentication.</p>
        </div>
        
        <div class="feature">
            <h3>📊 Queue Management</h3>
            <p>Robust email queuing with retry logic, priority handling, and persistent storage.</p>
        </div>
        
        <div class="warning">
            <strong>⚠️ Note:</strong> This installer requires administrator privileges to install system-wide components.
            Configuration files will be created in <code>/etc/ssmtp-mailer/</code> and logs in <code>/var/log/ssmtp-mailer/</code>.
        </div>
        
        <p>Click <strong>Continue</strong> to proceed with the installation.</p>
    </div>
</body>
</html>
EOF

    print_success "Welcome HTML created successfully"
}

# Function to create conclusion HTML
create_conclusion_html() {
    print_status "Creating conclusion HTML..."
    
    cat > "$RESOURCES_DIR/conclusion.html" << 'EOF'
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>Installation Complete</title>
    <style>
        body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; margin: 40px; }
        .header { text-align: center; margin-bottom: 30px; }
        .content { max-width: 600px; margin: 0 auto; }
        .success { background: #d4edda; border: 1px solid #c3e6cb; padding: 15px; border-radius: 8px; margin: 20px 0; }
        .next-steps { background: #e2e3e5; border: 1px solid #d6d8db; padding: 15px; border-radius: 8px; margin: 20px 0; }
        .code { background: #f8f9fa; border: 1px solid #e9ecef; padding: 10px; border-radius: 4px; font-family: monospace; }
    </style>
</head>
<body>
    <div class="content">
        <div class="header">
            <h1>🎉 Installation Complete!</h1>
            <p><strong>ssmtp-mailer has been successfully installed on your system.</strong></p>
        </div>
        
        <div class="success">
            <h3>✅ What was installed:</h3>
            <ul>
                <li>Binary: <code>/usr/local/bin/ssmtp-mailer</code></li>
                <li>Configuration: <code>/etc/ssmtp-mailer/</code></li>
                <li>Logs: <code>/var/log/ssmtp-mailer/</code></li>
                <li>Headers: <code>/usr/local/include/ssmtp-mailer/</code></li>
            </ul>
        </div>
        
        <div class="next-steps">
            <h3>🚀 Next Steps:</h3>
            <ol>
                <li><strong>Test the installation:</strong><br>
                    <span class="code">ssmtp-mailer --version</span></li>
                <li><strong>Edit configuration:</strong><br>
                    <span class="code">sudo nano /etc/ssmtp-mailer/ssmtp-mailer.conf</span></li>
                <li><strong>View logs:</strong><br>
                    <span class="code">tail -f /var/log/ssmtp-mailer/ssmtp-mailer.log</span></li>
                <li><strong>Get help:</strong><br>
                    <span class="code">ssmtp-mailer --help</span></li>
            </ol>
        </div>
        
        <p><strong>Thank you for choosing ssmtp-mailer!</strong></p>
        <p>For more information, visit the project documentation or GitHub repository.</p>
    </div>
</body>
</html>
EOF

    print_success "Conclusion HTML created successfully"
}

# Function to create license file
create_license_file() {
    print_status "Creating license file..."
    
    cat > "$RESOURCES_DIR/license.txt" << 'EOF'
ssmtp-mailer License

Copyright (c) 2024 ssmtp-mailer Project

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied. See the License for the specific language governing
permissions and limitations under the License.

For more information, please visit: https://github.com/blburns/ssmtp-mailer
EOF

    print_success "License file created successfully"
}

# Function to build component package
build_component_package() {
    print_status "Building component package..."
    
    local component_pkg="$PKG_BUILD_DIR/$PACKAGE_TITLE-$PACKAGE_VERSION-component.pkg"
    
    pkgbuild \
        --root "$COMPONENT_DIR" \
        --scripts "$SCRIPTS_DIR" \
        --identifier "$PACKAGE_ID" \
        --version "$PACKAGE_VERSION" \
        --install-location "$PACKAGE_INSTALL_LOCATION" \
        --component-plist "$COMPONENT_DIR/component.plist" \
        "$component_pkg"
    
    if [ $? -eq 0 ]; then
        print_success "Component package built: $component_pkg"
        echo "$component_pkg"
    else
        print_error "Failed to build component package"
        exit 1
    fi
}

# Function to build final package
build_final_package() {
    local component_pkg="$1"
    local final_pkg="$DIST_DIR/$PACKAGE_TITLE-$PACKAGE_VERSION.pkg"
    
    print_status "Building final package..."
    
    # Ensure dist directory exists
    mkdir -p "$DIST_DIR"
    
    productbuild \
        --distribution "$PKG_BUILD_DIR/distribution.xml" \
        --resources "$RESOURCES_DIR" \
        --package-path "$PKG_BUILD_DIR" \
        "$final_pkg"
    
    if [ $? -eq 0 ]; then
        print_success "Final package built: $final_pkg"
        
        # Show package info
        print_status "Package information:"
        pkgutil --expand "$final_pkg" /tmp/ssmtp-mailer-expand 2>/dev/null || true
        if [ -d "/tmp/ssmtp-mailer-expand" ]; then
            print_status "Package contents:"
            find "/tmp/ssmtp-mailer-expand" -type f | head -20
            rm -rf "/tmp/ssmtp-mailer-expand"
        fi
        
        # Show file size
        local size=$(du -h "$final_pkg" | cut -f1)
        print_status "Package size: $size"
        
        return 0
    else
        print_error "Failed to build final package"
        return 1
    fi
}

# Function to show build summary
show_build_summary() {
    echo ""
    echo "╔══════════════════════════════════════════════════════════════════════════════╗"
    echo "║                              PKG BUILD COMPLETE                             ║"
    echo "╚══════════════════════════════════════════════════════════════════════════════╝"
    echo ""
    echo "🎉 ssmtp-mailer PKG package has been successfully built!"
    echo ""
    echo "📦 Package Details:"
    echo "  • File:          $DIST_DIR/$PACKAGE_TITLE-$PACKAGE_VERSION.pkg"
    echo "  • Identifier:    $PACKAGE_ID"
    echo "  • Version:       $PACKAGE_VERSION"
    echo "  • Install Path:  $PACKAGE_INSTALL_LOCATION"
    echo ""
    echo "🔧 Installation:"
    echo "  • Double-click the .pkg file to install"
    echo "  • Or use: sudo installer -pkg '$DIST_DIR/$PACKAGE_TITLE-$PACKAGE_VERSION.pkg' -target /"
    echo ""
    echo "📚 Next Steps:"
    echo "  1. Test the PKG installation on a clean system"
    echo "  2. Distribute the PKG file to users"
    echo "  3. Consider code signing for production use"
    echo ""
}

# Main build function
build_pkg() {
    print_status "Starting PKG build process..."
    
    # Create PKG build structure
    create_pkg_structure
    
    # Copy build artifacts
    copy_build_artifacts
    
    # Create configuration files
    create_config_files
    
    # Create package info
    create_package_info
    
    # Create scripts
    create_postinstall_script
    create_preuninstall_script
    
    # Create resources
    create_welcome_html
    create_conclusion_html
    create_license_file
    
    # Create distribution XML
    create_distribution_xml
    
    # Build component package
    local component_pkg=$(build_component_package)
    
    # Build final package
    if build_final_package "$component_pkg"; then
        show_build_summary
    else
        print_error "PKG build failed"
        exit 1
    fi
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_usage
            exit 0
            ;;
        -v|--version)
            show_version
            exit 0
            ;;
        -b|--build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        -d|--dist-dir)
            DIST_DIR="$2"
            shift 2
            ;;
        -p|--pkg-dir)
            PKG_BUILD_DIR="$2"
            shift 2
            ;;
        -i|--install-location)
            PACKAGE_INSTALL_LOCATION="$2"
            shift 2
            ;;
        --clean)
            CLEAN_PKG=true
            shift
            ;;
        --no-scripts)
            SKIP_SCRIPTS=true
            shift
            ;;
        --verbose)
            VERBOSE=true
            shift
            ;;
        *)
            print_error "Unknown option: $1"
            show_usage
            exit 1
            ;;
    esac
done

# Main execution
main() {
    print_status "Starting ssmtp-mailer PKG build..."
    print_status "Project: $PROJECT_ROOT"
    print_status "Build directory: $BUILD_DIR"
    print_status "Distribution directory: $DIST_DIR"
    print_status "PKG build directory: $PKG_BUILD_DIR"
    
    # Check dependencies
    check_dependencies
    
    # Clean if requested
    clean_pkg_build "$CLEAN_PKG"
    
    # Build PKG
    build_pkg
}

# Run main function
main "$@"
