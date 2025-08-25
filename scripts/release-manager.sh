#!/bin/bash

# ssmtp-mailer Release Manager
# This script helps coordinate package building across different platforms
# and organizes the results into a structured release directory.

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
VERSION=$(grep '^VERSION =' "$PROJECT_ROOT/Makefile" | cut -d' ' -f3)
DIST_DIR="$PROJECT_ROOT/dist"
RELEASE_DIR="$DIST_DIR/releases/v$VERSION"

# Functions
print_header() {
    echo -e "${BLUE}╔══════════════════════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}║                        ssmtp-mailer Release Manager v$VERSION                    ║${NC}"
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

detect_platform() {
    case "$(uname -s)" in
        Linux*)     echo "linux" ;;
        Darwin*)    echo "macos" ;;
        CYGWIN*|MINGW*|MSYS*) echo "windows" ;;
        *)          echo "unknown" ;;
    esac
}

detect_linux_distro() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        echo "$ID"
    elif [ -f /etc/debian_version ]; then
        echo "debian"
    elif [ -f /etc/redhat-release ]; then
        echo "redhat"
    else
        echo "unknown"
    fi
}

create_release_structure() {
    print_info "Creating release directory structure..."
    
    mkdir -p "$RELEASE_DIR"/{linux,macos,windows,source,docs}
    
    print_success "Release structure created: $RELEASE_DIR"
    print_info "Directory structure:"
    tree "$RELEASE_DIR" 2>/dev/null || ls -la "$RELEASE_DIR"
}

build_linux_packages() {
    local platform=$(detect_platform)
    if [ "$platform" != "linux" ]; then
        print_warning "Not on Linux platform, skipping Linux package builds"
        return 0
    fi
    
    local distro=$(detect_linux_distro)
    print_info "Building Linux packages on $distro..."
    
    cd "$PROJECT_ROOT"
    
    # Clean and build
    make clean
    make build
    
    # Create packages
    cd build
    if cpack -G DEB; then
        print_success "DEB package created successfully"
    else
        print_warning "DEB package creation failed"
    fi
    
    if cpack -G RPM; then
        print_success "RPM package created successfully"
    else
        print_warning "RPM package creation failed"
    fi
    
    if cpack -G TGZ; then
        print_success "TGZ package created successfully"
    else
        print_warning "TGZ package creation failed"
    fi
    
    # Move packages to release directory
    if ls ssmtp-mailer-$VERSION-*.deb 1> /dev/null 2>&1; then
        mv ssmtp-mailer-$VERSION-*.deb "$RELEASE_DIR/linux/"
        print_success "DEB packages moved to release directory"
    fi
    
    if ls ssmtp-mailer-$VERSION-*.rpm 1> /dev/null 2>&1; then
        mv ssmtp-mailer-$VERSION-*.rpm "$RELEASE_DIR/linux/"
        print_success "RPM packages moved to release directory"
    fi
    
    if ls ssmtp-mailer-$VERSION-*.tar.gz 1> /dev/null 2>&1; then
        mv ssmtp-mailer-$VERSION-*.tar.gz "$RELEASE_DIR/linux/"
        print_success "TGZ packages moved to release directory"
    fi
    
    cd "$PROJECT_ROOT"
}

build_macos_packages() {
    local platform=$(detect_platform)
    if [ "$platform" != "macos" ]; then
        print_warning "Not on macOS platform, skipping macOS package builds"
        return 0
    fi
    
    print_info "Building macOS packages..."
    
    cd "$PROJECT_ROOT"
    
    # Use the macOS build script
    if [ -f "scripts/build-macos.sh" ]; then
        chmod +x scripts/build-macos.sh
        ./scripts/build-macos.sh
        
        # Move packages to release directory
        if ls dist/ssmtp-mailer-$VERSION-*.dmg 1> /dev/null 2>&1; then
            mv dist/ssmtp-mailer-$VERSION-*.dmg "$RELEASE_DIR/macos/"
            print_success "DMG packages moved to release directory"
        fi
        
        if ls dist/ssmtp-mailer-$VERSION-*.pkg 1> /dev/null 2>&1; then
            mv dist/ssmtp-mailer-$VERSION-*.pkg "$RELEASE_DIR/macos/"
            print_success "PKG packages moved to release directory"
        fi
    else
        print_error "macOS build script not found"
        return 1
    fi
    
    cd "$PROJECT_ROOT"
}

build_source_package() {
    print_info "Creating source package..."
    
    cd "$PROJECT_ROOT"
    
    if [ -d .git ]; then
        git archive --format=tar.gz --prefix=ssmtp-mailer-$VERSION/ HEAD > "$RELEASE_DIR/source/ssmtp-mailer-$VERSION-source.tar.gz"
        print_success "Source archive created: ssmtp-mailer-$VERSION-source.tar.gz"
    else
        print_warning "Not a git repository, skipping source archive"
    fi
    
    cd "$PROJECT_ROOT"
}

copy_documentation() {
    print_info "Copying documentation to release directory..."
    
    if [ -d "docs" ]; then
        cp -r docs "$RELEASE_DIR/"
        print_success "Documentation copied to release directory"
    else
        print_warning "Documentation directory not found"
    fi
}

create_release_notes() {
    print_info "Creating release notes..."
    
    cat > "$RELEASE_DIR/RELEASE_NOTES.md" << EOF
# ssmtp-mailer v$VERSION Release Notes

## Release Date
$(date '+%Y-%m-%d')

## What's New
- Complete package system with FHS compliance
- OAuth2 helper tools for multiple email providers
- Professional installer packages for all platforms
- Comprehensive documentation and examples
- Cross-platform build system

## Package Types
- **Linux**: DEB, RPM, TGZ packages
- **macOS**: DMG and PKG installers
- **Windows**: NSIS installer (MSI)
- **Source**: Complete source code archive

## Installation
See the documentation in the docs/ directory for detailed installation instructions.

## Build Information
- **Build Date**: $(date)
- **Build Platform**: $(detect_platform)
- **Git Commit**: $(git rev-parse HEAD 2>/dev/null || echo "Unknown")
- **Git Branch**: $(git branch --show-current 2>/dev/null || echo "Unknown")

## Package Contents
$(find "$RELEASE_DIR" -type f -name "*.deb" -o -name "*.rpm" -o -name "*.tar.gz" -o -name "*.dmg" -o -name "*.pkg" | sort | sed 's|.*/||' | sed 's/^/- /')
EOF

    print_success "Release notes created: RELEASE_NOTES.md"
}

create_checksums() {
    print_info "Creating checksums for all packages..."
    
    cd "$RELEASE_DIR"
    
    # Create SHA256 checksums
    find . -type f \( -name "*.deb" -o -name "*.rpm" -o -name "*.tar.gz" -o -name "*.dmg" -o -name "*.pkg" \) -exec sha256sum {} \; > SHA256SUMS
    
    # Create MD5 checksums (for compatibility)
    find . -type f \( -name "*.deb" -o -name "*.rpm" -o -name "*.tar.gz" -o -name "*.dmg" -o -name "*.pkg" \) -exec md5sum {} \; > MD5SUMS
    
    print_success "Checksums created: SHA256SUMS and MD5SUMS"
    
    cd "$PROJECT_ROOT"
}

show_release_summary() {
    print_success "Release v$VERSION completed successfully!"
    echo ""
    echo -e "${BLUE}Release Directory:${NC} $RELEASE_DIR"
    echo ""
    echo -e "${BLUE}Package Summary:${NC}"
    
    local total_packages=0
    
    if [ -d "$RELEASE_DIR/linux" ]; then
        local linux_count=$(ls "$RELEASE_DIR/linux"/*.{deb,rpm,tar.gz} 2>/dev/null | wc -l)
        echo "  Linux: $linux_count packages"
        total_packages=$((total_packages + linux_count))
    fi
    
    if [ -d "$RELEASE_DIR/macos" ]; then
        local macos_count=$(ls "$RELEASE_DIR/macos"/*.{dmg,pkg} 2>/dev/null | wc -l)
        echo "  macOS: $macos_count packages"
        total_packages=$((total_packages + macos_count))
    fi
    
    if [ -d "$RELEASE_DIR/source" ]; then
        local source_count=$(ls "$RELEASE_DIR/source"/*.tar.gz 2>/dev/null | wc -l)
        echo "  Source: $source_count packages"
        total_packages=$((total_packages + source_count))
    fi
    
    echo ""
    echo -e "${BLUE}Total Packages:${NC} $total_packages"
    echo ""
    echo -e "${BLUE}Files Created:${NC}"
    find "$RELEASE_DIR" -type f | sort | sed 's|.*/||' | sed 's/^/  /'
    echo ""
    echo -e "${GREEN}Release is ready for distribution! 🚀${NC}"
}

show_usage() {
    cat << EOF
Usage: $0 [OPTIONS]

Options:
    -h, --help          Show this help message
    -v, --version       Show version information
    --clean             Clean previous release before building
    --linux-only        Build only Linux packages
    --macos-only        Build only macOS packages
    --source-only       Create only source package
    --no-checksums      Skip checksum creation
    --no-docs           Skip documentation copying

Examples:
    $0                    # Build complete release for all platforms
    $0 --clean           # Clean and rebuild complete release
    $0 --linux-only      # Build only Linux packages
    $0 --macos-only      # Build only macOS packages

EOF
}

# Main execution
main() {
    local clean_build=false
    local linux_only=false
    local macos_only=false
    local source_only=false
    local create_checksums_flag=true
    local copy_docs_flag=true
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_usage
                exit 0
                ;;
            -v|--version)
                echo "ssmtp-mailer Release Manager v$VERSION"
                exit 0
                ;;
            --clean)
                clean_build=true
                shift
                ;;
            --linux-only)
                linux_only=true
                shift
                ;;
            --macos-only)
                macos_only=true
                shift
                ;;
            --source-only)
                source_only=true
                shift
                ;;
            --no-checksums)
                create_checksums_flag=false
                shift
                ;;
            --no-docs)
                copy_docs_flag=false
                shift
                ;;
            *)
                print_error "Unknown option: $1"
                show_usage
                exit 1
                ;;
        esac
    done
    
    print_header
    
    # Change to project root
    cd "$PROJECT_ROOT"
    
    # Clean if requested
    if [ "$clean_build" = true ]; then
        print_info "Cleaning previous build artifacts..."
        make clean
        rm -rf "$RELEASE_DIR"
    fi
    
    # Create release structure
    create_release_structure
    
    # Build packages based on options
    if [ "$source_only" = true ]; then
        build_source_package
    elif [ "$linux_only" = true ]; then
        build_linux_packages
    elif [ "$macos_only" = true ]; then
        build_macos_packages
    else
        # Build all packages
        build_linux_packages
        build_macos_packages
        build_source_package
    fi
    
    # Copy documentation if requested
    if [ "$copy_docs_flag" = true ]; then
        copy_documentation
    fi
    
    # Create release notes
    create_release_notes
    
    # Create checksums if requested
    if [ "$create_checksums_flag" = true ]; then
        create_checksums
    fi
    
    # Show summary
    show_release_summary
}

# Run main function with all arguments
main "$@"
