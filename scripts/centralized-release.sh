#!/bin/bash

# ssmtp-mailer Centralized Release Manager
# This script manages releases from a centralized location where packages from
# different systems (Linux, macOS, Windows) have been collected

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Script configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
VERSION=$(grep '^VERSION =' "$PROJECT_ROOT/Makefile" | cut -d' ' -f3)
DIST_DIR="$PROJECT_ROOT/dist"
RELEASE_DIR="$DIST_DIR/releases/v$VERSION"
CENTRAL_RELEASE_DIR="$DIST_DIR/centralized"

# Functions
print_header() {
    echo -e "${BLUE}╔══════════════════════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}║                ssmtp-mailer Centralized Release Manager v$VERSION              ║${NC}"
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
    echo -e "${CYAN}🔧 $1${NC}"
}

print_package() {
    echo -e "${PURPLE}📦 $1${NC}"
}

check_gh_cli() {
    if ! command -v gh &> /dev/null; then
        print_error "GitHub CLI (gh) is not installed"
        echo ""
        echo "Install GitHub CLI:"
        echo "  macOS: brew install gh"
        echo "  Ubuntu: sudo apt install gh"
        echo "  Windows: winget install GitHub.cli"
        echo ""
        echo "Then run: gh auth login"
        exit 1
    fi
    
    if ! gh auth status &> /dev/null; then
        print_error "Not authenticated with GitHub CLI"
        echo ""
        echo "Run: gh auth login"
        exit 1
    fi
    
    print_success "GitHub CLI is ready"
}

check_central_release_dir() {
    if [[ ! -d "$CENTRAL_RELEASE_DIR" ]]; then
        print_info "Creating centralized release directory..."
        mkdir -p "$CENTRAL_RELEASE_DIR"
        print_success "Created $CENTRAL_RELEASE_DIR"
    fi
    
    # Create version subdirectory
    local version_dir="$CENTRAL_RELEASE_DIR/v$VERSION"
    if [[ ! -d "$version_dir" ]]; then
        mkdir -p "$version_dir"
        print_success "Created $version_dir"
    fi
}

scan_for_packages() {
    local version_dir="$CENTRAL_RELEASE_DIR/v$VERSION"
    local packages=()
    
    print_step "Scanning for packages in centralized release directory..."
    
    # Look for packages in the centralized directory
    if [[ -d "$version_dir" ]]; then
        while IFS= read -r -d '' file; do
            if [[ -f "$file" ]]; then
                packages+=("$file")
            fi
        done < <(find "$version_dir" -type f \( -name "*.deb" -o -name "*.rpm" -o -name "*.tar.gz" -o -name "*.zip" -o -name "*.dmg" -o -name "*.pkg" -o -name "*.exe" -o -name "*.msi" \) -print0 2>/dev/null)
    fi
    
    # Also check the local dist directory
    if [[ -d "$RELEASE_DIR" ]]; then
        while IFS= read -r -d '' file; do
            if [[ -f "$file" ]]; then
                packages+=("$file")
            fi
        done < <(find "$RELEASE_DIR" -type f \( -name "*.deb" -o -name "*.rpm" -o -name "*.tar.gz" -o -name "*.zip" -o -name "*.dmg" -o -name "*.pkg" -o -name "*.exe" -o -name "*.msi" \) -print0 2>/dev/null)
    fi
    
    if [[ ${#packages[@]} -eq 0 ]]; then
        print_warning "No packages found in centralized or local release directories"
        return 1
    fi
    
    print_success "Found ${#packages[@]} package(s):"
    for package in "${packages[@]}"; do
        local filename=$(basename "$package")
        local size=$(du -h "$package" | cut -f1)
        print_package "$filename ($size)"
    done
    
    return 0
}

collect_packages() {
    local version_dir="$CENTRAL_RELEASE_DIR/v$VERSION"
    
    print_step "Collecting packages from local release directory..."
    
    if [[ ! -d "$RELEASE_DIR" ]]; then
        print_warning "Local release directory not found: $RELEASE_DIR"
        return 1
    fi
    
    # Copy all packages to centralized directory
    local copied=0
    while IFS= read -r -d '' file; do
        if [[ -f "$file" ]]; then
            local filename=$(basename "$file")
            local dest="$version_dir/$filename"
            
            if [[ ! -f "$dest" ]] || [[ "$file" -nt "$dest" ]]; then
                cp "$file" "$dest"
                print_package "Copied: $filename"
                ((copied++))
            else
                print_info "Skipped (already exists): $filename"
            fi
        fi
    done < <(find "$RELEASE_DIR" -type f \( -name "*.deb" -o -name "*.rpm" -o -name "*.tar.gz" -o -name "*.zip" -o -name "*.dmg" -o -name "*.pkg" -o -name "*.exe" -o -name "*.msi" \) -print0 2>/dev/null)
    
    if [[ $copied -gt 0 ]]; then
        print_success "Copied $copied package(s) to centralized directory"
    else
        print_info "No new packages to copy"
    fi
}

create_release_notes() {
    local version_dir="$CENTRAL_RELEASE_DIR/v$VERSION"
    local release_notes="$version_dir/RELEASE_NOTES.md"
    
    print_step "Creating comprehensive release notes..."
    
    cat > "$release_notes" << EOF
# ssmtp-mailer v$VERSION Release Notes

## Release Information
- **Version**: $VERSION
- **Release Date**: $(date '+%Y-%m-%d %H:%M:%S UTC')
- **Release Manager**: Centralized Release Script

## Packages Included

EOF
    
    # List all packages with details
    if [[ -d "$version_dir" ]]; then
        while IFS= read -r -d '' file; do
            if [[ -f "$file" ]]; then
                local filename=$(basename "$file")
                local size=$(du -h "$file" | cut -f1)
                local platform=$(get_platform_from_filename "$filename")
                local arch=$(get_architecture_from_filename "$filename")
                
                echo "- **$filename**" >> "$release_notes"
                echo "  - Platform: $platform" >> "$release_notes"
                echo "  - Architecture: $arch" >> "$release_notes"
                echo "  - Size: $size" >> "$release_notes"
                echo "" >> "$release_notes"
            fi
        done < <(find "$version_dir" -type f \( -name "*.deb" -o -name "*.rpm" -o -name "*.tar.gz" -o -name "*.zip" -o -name "*.dmg" -o -name "*.pkg" -o -name "*.exe" -o -name "*.msi" \) -print0 2>/dev/null)
    fi
    
    # Add standard release notes content
    cat >> "$release_notes" << EOF

## What's New in v$VERSION

- Enhanced email sending capabilities
- Improved SMTP and API client support
- Better error handling and logging
- Cross-platform compatibility improvements

## Supported Platforms

- **Linux**: Debian/Ubuntu (.deb), Red Hat/CentOS (.rpm), Generic (.tar.gz)
- **macOS**: Intel (.dmg, .pkg), Apple Silicon (.dmg, .pkg)
- **Windows**: Installer (.exe), MSI Package (.msi)
- **Source**: Source code archive (.tar.gz)

## Installation

### Linux (Debian/Ubuntu)
\`\`\`bash
sudo dpkg -i ssmtp-mailer-$VERSION-linux-amd64.deb
\`\`\`

### Linux (Red Hat/CentOS)
\`\`\`bash
sudo rpm -i ssmtp-mailer-$VERSION-linux-amd64.rpm
\`\`\`

### macOS
Double-click the .dmg file and follow the installation instructions.

### Windows
Run the .exe installer and follow the setup wizard.

## Documentation

For detailed documentation, visit: https://github.com/your-repo/ssmtp-mailer

## Support

If you encounter any issues, please:
1. Check the documentation
2. Search existing issues
3. Create a new issue with detailed information

## Changelog

See the [CHANGELOG.md](CHANGELOG.md) file for a complete list of changes.
EOF
    
    print_success "Release notes created: $release_notes"
}

get_platform_from_filename() {
    local filename="$1"
    
    case "$filename" in
        *linux*) echo "Linux" ;;
        *macOS*) echo "macOS" ;;
        *windows*) echo "Windows" ;;
        *source*) echo "Source" ;;
        *) echo "Unknown" ;;
    esac
}

get_architecture_from_filename() {
    local filename="$1"
    
    case "$filename" in
        *x86_64*|*amd64*) echo "x86_64/AMD64" ;;
        *aarch64*|*arm64*) echo "ARM64" ;;
        *i386*|*i686*) echo "i386/i686" ;;
        *source*) echo "Source" ;;
        *) echo "Unknown" ;;
    esac
}

check_release_exists() {
    local tag="v$VERSION"
    
    if gh release view "$tag" &> /dev/null; then
        print_warning "Release v$VERSION already exists"
        echo ""
        echo "Options:"
        echo "  1. Update existing release (add new files)"
        echo "  2. Delete and recreate release"
        echo "  3. Exit"
        echo ""
        read -p "Choose option (1-3): " choice
        
        case $choice in
            1)
                print_info "Will update existing release"
                RELEASE_ACTION="update"
                ;;
            2)
                print_info "Deleting existing release..."
                gh release delete "$tag" --yes
                print_success "Release deleted"
                RELEASE_ACTION="create"
                ;;
            3)
                print_info "Exiting..."
                exit 0
                ;;
            *)
                print_error "Invalid choice, exiting..."
                exit 1
                ;;
        esac
    else
        RELEASE_ACTION="create"
    fi
}

create_github_release() {
    local version_dir="$CENTRAL_RELEASE_DIR/v$VERSION"
    local tag="v$VERSION"
    local release_notes="$version_dir/RELEASE_NOTES.md"
    
    print_step "Creating GitHub release..."
    
    if [[ "$RELEASE_ACTION" == "create" ]]; then
        gh release create "$tag" \
            --title "ssmtp-mailer v$VERSION" \
            --notes-file "$release_notes" \
            --draft
        print_success "Draft release created"
    fi
    
    # Upload all packages
    print_step "Uploading packages to GitHub release..."
    local uploaded=0
    
    while IFS= read -r -d '' file; do
        if [[ -f "$file" ]]; then
            local filename=$(basename "$file")
            print_package "Uploading: $filename"
            
            if gh release upload "$tag" "$file" --clobber; then
                print_success "Uploaded: $filename"
                ((uploaded++))
            else
                print_error "Failed to upload: $filename"
            fi
        fi
    done < <(find "$version_dir" -type f \( -name "*.deb" -o -name "*.rpm" -o -name "*.tar.gz" -o -name "*.zip" -o -name "*.dmg" -o -name "*.pkg" -o -name "*.exe" -o -name "*.msi" \) -print0 2>/dev/null)
    
    if [[ $uploaded -gt 0 ]]; then
        print_success "Successfully uploaded $uploaded package(s)"
    fi
    
    # Publish the release
    print_step "Publishing release..."
    gh release edit "$tag" --draft=false
    print_success "Release v$VERSION published successfully!"
}

show_help() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -h, --help          Show this help message"
    echo "  -s, --scan          Scan for existing packages"
    echo "  -c, --collect       Collect packages from local release directory"
    echo "  -r, --release       Create and publish GitHub release"
    echo "  -a, --all           Run all steps (scan, collect, release)"
    echo "  -v, --version       Show version information"
    echo ""
    echo "Examples:"
    echo "  $0 --scan           # Just scan for packages"
    echo "  $0 --collect        # Collect packages from local directory"
    echo "  $0 --release        # Create GitHub release with existing packages"
    echo "  $0 --all            # Run complete release process"
    echo ""
    echo "This script is designed to work with packages collected from multiple systems."
    echo "Place packages from different platforms in: $CENTRAL_RELEASE_DIR/v$VERSION/"
}

main() {
    print_header
    
    # Parse command line arguments
    local do_scan=false
    local do_collect=false
    local do_release=false
    
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_help
                exit 0
                ;;
            -s|--scan)
                do_scan=true
                shift
                ;;
            -c|--collect)
                do_collect=true
                shift
                ;;
            -r|--release)
                do_release=true
                shift
                ;;
            -a|--all)
                do_scan=true
                do_collect=true
                do_release=true
                shift
                ;;
            -v|--version)
                echo "ssmtp-mailer Centralized Release Manager v$VERSION"
                exit 0
                ;;
            *)
                print_error "Unknown option: $1"
                show_help
                exit 1
                ;;
        esac
    done
    
    # If no options specified, show help
    if [[ "$do_scan" == false && "$do_collect" == false && "$do_release" == false ]]; then
        show_help
        exit 0
    fi
    
    # Check prerequisites
    check_gh_cli
    check_central_release_dir
    
    # Execute requested actions
    if [[ "$do_scan" == true ]]; then
        scan_for_packages
    fi
    
    if [[ "$do_collect" == true ]]; then
        collect_packages
    fi
    
    if [[ "$do_release" == true ]]; then
        if scan_for_packages; then
            create_release_notes
            check_release_exists
            create_github_release
        else
            print_error "No packages found. Cannot create release."
            exit 1
        fi
    fi
    
    print_success "Centralized release process completed!"
}

# Run main function with all arguments
main "$@"
