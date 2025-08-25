#!/bin/bash

# ssmtp-mailer GitHub Release Uploader
# This script helps upload packages to GitHub releases

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
    echo -e "${BLUE}║                    ssmtp-mailer GitHub Release Uploader v$VERSION                ║${NC}"
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
                RELEASE_ACTION="delete"
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

create_release() {
    local tag="v$VERSION"
    local release_notes="$RELEASE_DIR/RELEASE_NOTES.md"
    
    print_info "Creating GitHub release v$VERSION..."
    
    if [ -f "$release_notes" ]; then
        print_info "Using existing release notes: $release_notes"
        gh release create "$tag" \
            --title "ssmtp-mailer v$VERSION" \
            --notes-file "$release_notes" \
            --target main
    else
        print_info "Creating release with default notes..."
        gh release create "$tag" \
            --title "ssmtp-mailer v$VERSION" \
            --notes "Release v$VERSION of ssmtp-mailer with complete package system and OAuth2 tools" \
            --target main
    fi
    
    print_success "GitHub release created: v$VERSION"
}

upload_packages() {
    local tag="v$VERSION"
    
    print_info "Uploading packages to GitHub release..."
    
    # Upload Linux packages
    if [ -d "$RELEASE_DIR/linux" ]; then
        local linux_packages=$(find "$RELEASE_DIR/linux" -type f \( -name "*.deb" -o -name "*.rpm" -o -name "*.tar.gz" \) 2>/dev/null)
        if [ -n "$linux_packages" ]; then
            print_info "Uploading Linux packages..."
            echo "$linux_packages" | while read -r package; do
                if [ -f "$package" ]; then
                    gh release upload "$tag" "$package"
                    print_success "Uploaded: $(basename "$package")"
                fi
            done
        fi
    fi
    
    # Upload macOS packages
    if [ -d "$RELEASE_DIR/macos" ]; then
        local macos_packages=$(find "$RELEASE_DIR/macos" -type f \( -name "*.dmg" -o -name "*.pkg" \) 2>/dev/null)
        if [ -n "$macos_packages" ]; then
            print_info "Uploading macOS packages..."
            echo "$macos_packages" | while read -r package; do
                if [ -f "$package" ]; then
                    gh release upload "$tag" "$package" --clobber
                    print_success "Uploaded: $(basename "$package")"
                fi
            done
        fi
    fi
    
    # Upload Windows packages
    if [ -d "$RELEASE_DIR/windows" ]; then
        local windows_packages=$(find "$RELEASE_DIR/windows" -type f -name "*.exe" 2>/dev/null)
        if [ -n "$windows_packages" ]; then
            print_info "Uploading Windows packages..."
            echo "$windows_packages" | while read -r package; do
                if [ -f "$package" ]; then
                    gh release upload "$tag" "$package"
                    print_success "Uploaded: $(basename "$package")"
                fi
            done
        fi
    fi
    
    # Upload source packages
    if [ -d "$RELEASE_DIR/source" ]; then
        local source_packages=$(find "$RELEASE_DIR/source" -type f -name "*.tar.gz" 2>/dev/null)
        if [ -n "$source_packages" ]; then
            print_info "Uploading source packages..."
            echo "$source_packages" | while read -r package; do
                if [ -f "$package" ]; then
                    gh release upload "$tag" "$package"
                    print_success "Uploaded: $(basename "$package")"
                fi
            done
        fi
    fi
    
    # Upload documentation
    if [ -d "$RELEASE_DIR/docs" ]; then
        print_info "Uploading documentation..."
        find "$RELEASE_DIR/docs" -type f | while read -r doc_file; do
            if [ -f "$doc_file" ]; then
                gh release upload "$tag" "$doc_file"
                print_success "Documentation uploaded: $(basename "$doc_file")"
            fi
        done
    fi
    
    # Upload checksums
    if [ -f "$RELEASE_DIR/SHA256SUMS" ]; then
        gh release upload "$tag" "$RELEASE_DIR/SHA256SUMS"
        print_success "SHA256SUMS uploaded"
    fi
    
    if [ -f "$RELEASE_DIR/MD5SUMS" ]; then
        gh release upload "$tag" "$RELEASE_DIR/MD5SUMS"
        print_success "MD5SUMS uploaded"
    fi
    
    print_success "All packages uploaded successfully!"
}

show_release_url() {
    local tag="v$VERSION"
    local repo_url=$(gh repo view --json url -q .url)
    local release_url="$repo_url/releases/tag/$tag"
    
    echo ""
    echo -e "${BLUE}🎉 Release Complete!${NC}"
    echo ""
    echo -e "${BLUE}Release URL:${NC} $release_url"
    echo ""
    echo -e "${BLUE}Next Steps:${NC}"
    echo "  1. Visit the release URL to verify all packages"
    echo "  2. Share the release URL with users"
    echo "  3. Update your project documentation"
    echo "  4. Announce the release on social media/forums"
    echo ""
    echo -e "${GREEN}Your packages are now available for download! 🚀${NC}"
}

show_usage() {
    cat << EOF
Usage: $0 [OPTIONS]

Options:
    -h, --help          Show this help message
    -v, --version       Show version information
    --force             Force upload even if release exists
    --dry-run           Show what would be uploaded without actually uploading

Examples:
    $0                    # Upload packages to GitHub release
    $0 --force           # Force upload (overwrite existing release)
    $0 --dry-run         # Show what would be uploaded

EOF
}

# Main execution
main() {
    local force_upload=false
    local dry_run=false
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_usage
                exit 0
                ;;
            -v|--version)
                echo "ssmtp-mailer GitHub Release Uploader v$VERSION"
                exit 0
                ;;
            --force)
                force_upload=true
                shift
                ;;
            --dry-run)
                dry_run=true
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
    
    # Check prerequisites
    check_gh_cli
    
    # Check if release directory exists
    if [ ! -d "$RELEASE_DIR" ]; then
        print_error "Release directory not found: $RELEASE_DIR"
        echo ""
        echo "Please build packages first:"
        echo "  make release"
        echo "  # or"
        echo "  ./scripts/release-manager.sh"
        exit 1
    fi
    
    if [ "$dry_run" = true ]; then
        print_info "DRY RUN MODE - No files will be uploaded"
        echo ""
        echo "Files that would be uploaded:"
        find "$RELEASE_DIR" -type f | sort
        exit 0
    fi
    
    # Check if release exists (unless forcing)
    RELEASE_ACTION="create"
    if [ "$force_upload" = false ]; then
        check_release_exists
    fi
    
    # Handle release creation based on user choice
    case "$RELEASE_ACTION" in
        "create")
            create_release
            ;;
        "update")
            print_info "Using existing release v$VERSION"
            ;;
        "delete")
            create_release
            ;;
        *)
            print_error "Invalid release action: $RELEASE_ACTION"
            exit 1
            ;;
    esac
    
    # Upload packages
    upload_packages
    
    # Show results
    show_release_url
}

# Run main function with all arguments
main "$@"
