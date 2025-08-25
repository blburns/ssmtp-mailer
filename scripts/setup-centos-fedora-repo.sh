#!/bin/bash

# CentOS Custom Repository Setup Script
# This script sets up custom YUM repositories to access EPEL 9 packages on CentOS

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Script configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
REPO_FILE="$PROJECT_ROOT/etc/yum.repos.d/custom.repo"
TARGET_REPO="/etc/yum.repos.d/custom.repo"

# Functions
print_header() {
    echo -e "${BLUE}╔══════════════════════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}║                CentOS Custom Repository Setup Script                        ║${NC}"
    echo -e "${BLUE}╚══════════════════════════════════════════════════════════════════════════════╝${NC}"
    echo ""
}

print_info() {
    echo -e "${GREEN}ℹ️  $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

print_error() {
    echo -e "${RED}❌ $1${NC}"
}

print_success() {
    echo -e "${GREEN}✅ $1${NC}"
}

print_step() {
    echo -e "${BLUE}🔧 $1${NC}"
}

check_root() {
    if [[ $EUID -ne 0 ]]; then
        print_error "This script must be run as root (use sudo)"
        exit 1
    fi
}

check_centos() {
    if [[ ! -f /etc/redhat-release ]]; then
        print_error "This script is designed for CentOS/RHEL systems"
        exit 1
    fi
    
    local distro=$(cat /etc/redhat-release)
    print_info "Detected distribution: $distro"
}

check_repo_file() {
    if [[ ! -f "$REPO_FILE" ]]; then
        print_error "Repository file not found: $REPO_FILE"
        print_info "Make sure you're running this script from the project root directory"
        exit 1
    fi
    
    print_success "Repository file found: $REPO_FILE"
}

install_repo_file() {
    print_step "Installing custom repository file..."
    
    # Copy repository file
    cp "$REPO_FILE" "$TARGET_REPO"
    chmod 644 "$TARGET_REPO"
    
    print_success "Repository file installed: $TARGET_REPO"
}

import_gpg_keys() {
    print_step "Importing GPG keys..."
    
    # Import EPEL 9 GPG key
    print_info "Importing EPEL 9 GPG key..."
    rpm --import https://dl.fedoraproject.org/pub/epel/RPM-GPG-KEY-EPEL-9
    
    # Import EPEL 8 GPG key (for older systems)
    print_info "Importing EPEL 8 GPG key..."
    rpm --import https://dl.fedoraproject.org/pub/epel/RPM-GPG-KEY-EPEL-8
    
    print_success "GPG keys imported successfully"
}

clear_yum_cache() {
    print_step "Clearing YUM cache..."
    
    yum clean all
    yum makecache
    
    print_success "YUM cache updated"
}

test_repository() {
    print_step "Testing repository access..."
    
    # Test if we can search in the EPEL 9 repository
    if yum --enablerepo=epel9 search cmake &>/dev/null; then
        print_success "Repository test successful"
    else
        print_warning "Repository test failed - you may need to enable it manually"
        print_info "Try: sudo yum-config-manager --enable epel9"
    fi
}

show_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -h, --help          Show this help message"
    echo "  -t, --test          Test repository access after setup"
    echo "  -e, --enable        Enable repository after setup"
    echo "  -a, --all           Run complete setup with test and enable"
    echo ""
    echo "Examples:"
    echo "  sudo $0              # Basic setup"
    echo "  sudo $0 --test       # Setup and test"
    echo "  sudo $0 --all        # Complete setup with test and enable"
    echo ""
    echo "⚠️  WARNING: This script modifies system package repositories."
    echo "   Use with caution and test thoroughly in a development environment."
}

main() {
    print_header
    
    # Parse command line arguments
    local do_test=false
    local do_enable=false
    
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_usage
                exit 0
                ;;
            -t|--test)
                do_test=true
                shift
                ;;
            -e|--enable)
                do_enable=true
                shift
                ;;
            -a|--all)
                do_test=true
                do_enable=true
                shift
                ;;
            *)
                print_error "Unknown option: $1"
                show_usage
                exit 1
                ;;
        esac
    done
    
    # Check prerequisites
    check_root
    check_centos
    check_repo_file
    
    # Install repository
    install_repo_file
    import_gpg_keys
    clear_yum_cache
    
    # Test if requested
    if [[ "$do_test" == true ]]; then
        test_repository
    fi
    
    # Enable if requested
    if [[ "$do_enable" == true ]]; then
        print_step "Enabling EPEL 9 repository..."
        yum-config-manager --enable epel9
        print_success "Repository enabled"
    fi
    
    print_success "EPEL 9 repository setup completed!"
    echo ""
    print_info "Repository file: $TARGET_REPO"
    print_info "To enable: sudo yum-config-manager --enable epel9"
    print_info "To disable: sudo yum-config-manager --disable epel9"
    echo ""
    print_warning "Remember to disable the repository after building to maintain system stability"
}

# Run main function with all arguments
main "$@"
