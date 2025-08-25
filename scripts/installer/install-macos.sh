#!/bin/bash

# ssmtp-mailer macOS Installer Script
# This script installs ssmtp-mailer from a DMG package

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
INSTALL_PREFIX="/usr/local"
CONFIG_DIR="/etc/ssmtp-mailer"
LOG_DIR="/var/log/ssmtp-mailer"
SERVICE_USER="ssmtp-mailer"
SERVICE_GROUP="ssmtp-mailer"

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

Install ssmtp-mailer on macOS from a DMG package.

OPTIONS:
    -h, --help              Show this help message
    -v, --version           Show version information
    -p, --prefix PATH       Installation prefix (default: /usr/local)
    -c, --config-dir PATH   Configuration directory (default: /etc/ssmtp-mailer)
    -l, --log-dir PATH      Log directory (default: /var/log/ssmtp-mailer)
    -u, --user USER         Service user (default: ssmtp-mailer)
    -g, --group GROUP       Service group (default: ssmtp-mailer)
    --dry-run               Show what would be installed without actually installing
    --uninstall             Uninstall ssmtp-mailer
    --upgrade               Upgrade existing installation

EXAMPLES:
    $0                        # Install with default settings
    $0 -p /opt               # Install to /opt prefix
    $0 --dry-run             # Show installation plan
    $0 --uninstall           # Remove ssmtp-mailer
    $0 --upgrade             # Upgrade existing installation

EOF
}

# Function to show version
show_version() {
    echo "ssmtp-mailer macOS installer v1.0.0"
}

# Function to check if running as root
check_root() {
    if [ "$EUID" -ne 0 ]; then
        print_error "This script must be run as root (use sudo)"
        exit 1
    fi
}

# Function to check macOS version
check_macos_version() {
    local macos_version=$(sw_vers -productVersion)
    local major_version=$(echo $macos_version | cut -d. -f1)
    local minor_version=$(echo $macos_version | cut -d. -f2)
    
    if [ "$major_version" -lt 11 ] || ([ "$major_version" -eq 11 ] && [ "$minor_version" -lt 0 ]); then
        print_error "macOS Big Sur 11.0+ is required. Current version: $macos_version"
        exit 1
    fi
    
    print_success "macOS version check passed: $macos_version"
}

# Function to find DMG file
find_dmg_file() {
    local dmg_file=""
    
    # Look in dist directory first
    if [ -d "$PROJECT_ROOT/dist" ]; then
        dmg_file=$(find "$PROJECT_ROOT/dist" -name "*.dmg" | head -1)
    fi
    
    # If not found, look in current directory
    if [ -z "$dmg_file" ]; then
        dmg_file=$(find . -name "*.dmg" | head -1)
    fi
    
    if [ -z "$dmg_file" ]; then
        print_error "No DMG file found. Please build the package first with 'make package-dmg'"
        exit 1
    fi
    
    print_status "Found DMG file: $dmg_file"
    echo "$dmg_file"
}

# Function to mount DMG
mount_dmg() {
    local dmg_file="$1"
    local mount_point="/tmp/ssmtp-mailer-install"
    
    # Create mount point
    mkdir -p "$mount_point"
    
    # Mount DMG
    print_status "Mounting DMG file..."
    hdiutil attach "$dmg_file" -mountpoint "$mount_point" -readonly
    
    echo "$mount_point"
}

# Function to unmount DMG
unmount_dmg() {
    local mount_point="$1"
    
    if [ -n "$mount_point" ] && [ -d "$mount_point" ]; then
        print_status "Unmounting DMG..."
        hdiutil detach "$mount_point" 2>/dev/null || true
        rmdir "$mount_point" 2>/dev/null || true
    fi
}

# Function to create directories
create_directories() {
    print_status "Creating installation directories..."
    
    # Create main directories
    mkdir -p "$INSTALL_PREFIX/bin"
    mkdir -p "$INSTALL_PREFIX/lib"
    mkdir -p "$INSTALL_PREFIX/include/ssmtp-mailer"
    mkdir -p "$CONFIG_DIR"
    mkdir -p "$LOG_DIR"
    
    # Create configuration subdirectories
    mkdir -p "$CONFIG_DIR/templates"
    mkdir -p "$CONFIG_DIR/ssl"
    mkdir -p "$CONFIG_DIR/auth"
    
    print_success "Directories created successfully"
}

# Function to copy files
copy_files() {
    local mount_point="$1"
    
    print_status "Copying application files..."
    
    # Copy binary
    if [ -f "$mount_point/ssmtp-mailer" ]; then
        cp "$mount_point/ssmtp-mailer" "$INSTALL_PREFIX/bin/"
        chmod +x "$INSTALL_PREFIX/bin/ssmtp-mailer"
        print_success "Binary installed to $INSTALL_PREFIX/bin/ssmtp-mailer"
    else
        print_error "Binary not found in DMG"
        exit 1
    fi
    
    # Copy library if it exists
    if [ -f "$mount_point/libssmtp-mailer.dylib" ]; then
        cp "$mount_point/libssmtp-mailer.dylib" "$INSTALL_PREFIX/lib/"
        print_success "Library installed to $INSTALL_PREFIX/lib/"
    fi
    
    # Copy headers if they exist
    if [ -d "$mount_point/include" ]; then
        cp -r "$mount_point/include"/* "$INSTALL_PREFIX/include/ssmtp-mailer/"
        print_success "Headers installed to $INSTALL_PREFIX/include/ssmtp-mailer/"
    fi
}

# Function to create configuration files
create_config_files() {
    print_status "Creating configuration files..."
    
    # Main configuration file
    cat > "$CONFIG_DIR/ssmtp-mailer.conf" << 'EOF'
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

    # Email template configuration
    cat > "$CONFIG_DIR/templates/email.conf" << 'EOF'
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
    cat > "$CONFIG_DIR/ssl/ssl.conf" << 'EOF'
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
    cat > "$CONFIG_DIR/auth/auth.conf" << 'EOF'
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

# Function to set permissions
set_permissions() {
    print_status "Setting file permissions..."
    
    # Set ownership
    chown -R root:wheel "$CONFIG_DIR"
    chown -R root:wheel "$LOG_DIR"
    
    # Set directory permissions
    chmod 755 "$CONFIG_DIR"
    chmod 755 "$LOG_DIR"
    chmod 755 "$CONFIG_DIR/templates"
    chmod 755 "$CONFIG_DIR/ssl"
    chmod 755 "$CONFIG_DIR/auth"
    
    # Set file permissions
    chmod 644 "$CONFIG_DIR"/*.conf
    chmod 644 "$CONFIG_DIR/templates"/*.conf
    chmod 644 "$CONFIG_DIR/ssl"/*.conf
    chmod 644 "$CONFIG_DIR/auth"/*.conf
    
    print_success "Permissions set successfully"
}

# Function to create symlinks
create_symlinks() {
    print_status "Creating symlinks..."
    
    # Create symlink in /usr/bin for easier access
    if [ ! -L "/usr/bin/ssmtp-mailer" ]; then
        ln -sf "$INSTALL_PREFIX/bin/ssmtp-mailer" "/usr/bin/ssmtp-mailer"
        print_success "Symlink created: /usr/bin/ssmtp-mailer"
    fi
    
    # Create symlink for library
    if [ -f "$INSTALL_PREFIX/lib/libssmtp-mailer.dylib" ]; then
        if [ ! -L "/usr/lib/libssmtp-mailer.dylib" ]; then
            ln -sf "$INSTALL_PREFIX/lib/libssmtp-mailer.dylib" "/usr/lib/libssmtp-mailer.dylib"
            print_success "Library symlink created: /usr/lib/libssmtp-mailer.dylib"
        fi
    fi
}

# Function to verify installation
verify_installation() {
    print_status "Verifying installation..."
    
    local errors=0
    
    # Check binary
    if [ ! -f "$INSTALL_PREFIX/bin/ssmtp-mailer" ]; then
        print_error "Binary not found: $INSTALL_PREFIX/bin/ssmtp-mailer"
        ((errors++))
    fi
    
    # Check configuration
    if [ ! -d "$CONFIG_DIR" ]; then
        print_error "Configuration directory not found: $CONFIG_DIR"
        ((errors++))
    fi
    
    # Check log directory
    if [ ! -d "$LOG_DIR" ]; then
        print_error "Log directory not found: $LOG_DIR"
        ((errors++))
    fi
    
    # Test binary
    if [ -f "$INSTALL_PREFIX/bin/ssmtp-mailer" ]; then
        if "$INSTALL_PREFIX/bin/ssmtp-mailer" --version >/dev/null 2>&1; then
            print_success "Binary test passed"
        else
            print_warning "Binary test failed (may be expected for some options)"
        fi
    fi
    
    if [ $errors -eq 0 ]; then
        print_success "Installation verification completed successfully"
    else
        print_error "Installation verification found $errors error(s)"
        return 1
    fi
}

# Function to show installation summary
show_installation_summary() {
    echo ""
    echo "╔══════════════════════════════════════════════════════════════════════════════╗"
    echo "║                              INSTALLATION COMPLETE                           ║"
    echo "╚══════════════════════════════════════════════════════════════════════════════╝"
    echo ""
    echo "🎉 ssmtp-mailer has been successfully installed!"
    echo ""
    echo "📁 Installation Details:"
    echo "  • Binary:        $INSTALL_PREFIX/bin/ssmtp-mailer"
    echo "  • Configuration: $CONFIG_DIR/"
    echo "  • Logs:          $LOG_DIR/"
    echo "  • Headers:       $INSTALL_PREFIX/include/ssmtp-mailer/"
    echo ""
    echo "🔧 Next Steps:"
    echo "  1. Edit configuration: sudo nano $CONFIG_DIR/ssmtp-mailer.conf"
    echo "  2. Test installation: ssmtp-mailer --version"
    echo "  3. View logs: tail -f $LOG_DIR/ssmtp-mailer.log"
    echo ""
    echo "📚 Documentation:"
    echo "  • Configuration: $CONFIG_DIR/"
    echo "  • Project docs:  $PROJECT_ROOT/docs/"
    echo ""
    echo "💡 Tip: Use 'ssmtp-mailer --help' to see available options"
    echo ""
}

# Function to uninstall
uninstall() {
    print_status "Uninstalling ssmtp-mailer..."
    
    # Remove binary
    if [ -f "$INSTALL_PREFIX/bin/ssmtp-mailer" ]; then
        rm -f "$INSTALL_PREFIX/bin/ssmtp-mailer"
        print_success "Binary removed"
    fi
    
    # Remove library
    if [ -f "$INSTALL_PREFIX/lib/libssmtp-mailer.dylib" ]; then
        rm -f "$INSTALL_PREFIX/lib/libssmtp-mailer.dylib"
        print_success "Library removed"
    fi
    
    # Remove headers
    if [ -d "$INSTALL_PREFIX/include/ssmtp-mailer" ]; then
        rm -rf "$INSTALL_PREFIX/include/ssmtp-mailer"
        print_success "Headers removed"
    fi
    
    # Remove symlinks
    if [ -L "/usr/bin/ssmtp-mailer" ]; then
        rm -f "/usr/bin/ssmtp-mailer"
        print_success "Binary symlink removed"
    fi
    
    if [ -L "/usr/lib/libssmtp-mailer.dylib" ]; then
        rm -f "/usr/lib/libssmtp-mailer.dylib"
        print_success "Library symlink removed"
    fi
    
    # Remove configuration (ask user)
    if [ -d "$CONFIG_DIR" ]; then
        read -p "Remove configuration directory $CONFIG_DIR? [y/N]: " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            rm -rf "$CONFIG_DIR"
            print_success "Configuration removed"
        else
            print_status "Configuration preserved at $CONFIG_DIR"
        fi
    fi
    
    # Remove logs (ask user)
    if [ -d "$LOG_DIR" ]; then
        read -p "Remove log directory $LOG_DIR? [y/N]: " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            rm -rf "$LOG_DIR"
            print_success "Logs removed"
        else
            print_status "Logs preserved at $LOG_DIR"
        fi
    fi
    
    print_success "Uninstallation completed"
}

# Function to show dry run
dry_run() {
    print_status "DRY RUN - No files will be modified"
    echo ""
    echo "📋 Installation Plan:"
    echo "  • Install binary to: $INSTALL_PREFIX/bin/ssmtp-mailer"
    echo "  • Create config dir: $CONFIG_DIR/"
    echo "  • Create log dir:    $LOG_DIR/"
    echo "  • Create symlinks:   /usr/bin/ssmtp-mailer"
    echo ""
    echo "🔧 Configuration files to be created:"
    echo "  • $CONFIG_DIR/ssmtp-mailer.conf"
    echo "  • $CONFIG_DIR/templates/email.conf"
    echo "  • $CONFIG_DIR/ssl/ssl.conf"
    echo "  • $CONFIG_DIR/auth/auth.conf"
    echo ""
    echo "📚 Documentation:"
    echo "  • Project docs: $PROJECT_ROOT/docs/"
    echo ""
}

# Main installation function
install() {
    local dmg_file=""
    local mount_point=""
    
    print_status "Starting ssmtp-mailer installation..."
    
    # Find DMG file
    dmg_file=$(find_dmg_file)
    
    # Mount DMG
    mount_point=$(mount_dmg "$dmg_file")
    
    # Trap to ensure DMG is unmounted on exit
    trap 'unmount_dmg "$mount_point"' EXIT
    
    # Create directories
    create_directories
    
    # Copy files
    copy_files "$mount_point"
    
    # Create configuration files
    create_config_files
    
    # Set permissions
    set_permissions
    
    # Create symlinks
    create_symlinks
    
    # Verify installation
    verify_installation
    
    # Show summary
    show_installation_summary
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
        -p|--prefix)
            INSTALL_PREFIX="$2"
            shift 2
            ;;
        -c|--config-dir)
            CONFIG_DIR="$2"
            shift 2
            ;;
        -l|--log-dir)
            LOG_DIR="$2"
            shift 2
            ;;
        -u|--user)
            SERVICE_USER="$2"
            shift 2
            ;;
        -g|--group)
            SERVICE_GROUP="$2"
            shift 2
            ;;
        --dry-run)
            DRY_RUN=true
            shift
            ;;
        --uninstall)
            UNINSTALL=true
            shift
            ;;
        --upgrade)
            UPGRADE=true
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
    # Check if running as root
    check_root
    
    # Check macOS version
    check_macos_version
    
    if [ "$DRY_RUN" = true ]; then
        dry_run
    elif [ "$UNINSTALL" = true ]; then
        uninstall
    elif [ "$UPGRADE" = true ]; then
        print_status "Upgrading existing installation..."
        # For now, just reinstall
        install
    else
        install
    fi
}

# Run main function
main "$@"
