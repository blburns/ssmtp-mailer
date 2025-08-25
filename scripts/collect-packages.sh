#!/bin/bash

# ssmtp-mailer Package Collection Helper
# This script helps collect packages from different build systems and prepare them
# for centralized release management

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
NC='\033[0m' # No Color

# Script configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
VERSION=$(grep '^VERSION =' "$PROJECT_ROOT/Makefile" | cut -d' ' -f3)
DIST_DIR="$PROJECT_ROOT/dist"
RELEASE_DIR="$DIST_DIR/releases/v$VERSION"
COLLECTION_DIR="$DIST_DIR/collections"

# Functions
print_header() {
    echo -e "${BLUE}╔══════════════════════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}║                ssmtp-mailer Package Collection Helper v$VERSION                ║${NC}"
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

detect_platform() {
    case "$(uname -s)" in
        Linux*)
            if command -v dpkg &> /dev/null; then
                echo "linux-debian"
            elif command -v rpm &> /dev/null; then
                echo "linux-redhat"
            else
                echo "linux-generic"
            fi
            ;;
        Darwin*)
            if [[ "$(uname -m)" == "arm64" ]]; then
                echo "macos-arm64"
            else
                echo "macos-x86_64"
            fi
            ;;
        MINGW*|MSYS*|CYGWIN*)
            echo "windows"
            ;;
        *)
            echo "unknown"
            ;;
    esac
}

detect_architecture() {
    case "$(uname -m)" in
        x86_64|amd64) echo "x86_64" ;;
        aarch64|arm64) echo "arm64" ;;
        i386|i686) echo "i386" ;;
        *) echo "unknown" ;;
    esac
}

create_collection_structure() {
    local platform=$(detect_platform)
    local arch=$(detect_architecture)
    
    # Create main collection directory
    if [[ ! -d "$COLLECTION_DIR" ]]; then
        mkdir -p "$COLLECTION_DIR"
    fi
    
    # Create version subdirectory
    local version_dir="$COLLECTION_DIR/v$VERSION"
    if [[ ! -d "$version_dir" ]]; then
        mkdir -p "$version_dir"
    fi
    
    # Create platform subdirectory
    local platform_dir="$version_dir/$platform"
    if [[ ! -d "$platform_dir" ]]; then
        mkdir -p "$platform_dir"
    fi
    
    echo "$platform_dir"
}

collect_packages() {
    local platform_dir="$1"
    local platform=$(detect_platform)
    
    print_step "Collecting packages for platform: $platform"
    
    if [[ ! -d "$RELEASE_DIR" ]]; then
        print_warning "Local release directory not found: $RELEASE_DIR"
        print_info "No packages to collect from local build"
        return 1
    fi
    
    # Copy all packages to collection directory
    local copied=0
    
    # Look for packages in platform-specific subdirectories
    local platform_subdirs=("macos" "linux" "windows" "source")
    for subdir in "${platform_subdirs[@]}"; do
        local subdir_path="$RELEASE_DIR/$subdir"
        if [[ -d "$subdir_path" ]]; then
            while IFS= read -r -d '' file; do
                if [[ -f "$file" ]]; then
                    local filename=$(basename "$file")
                    local dest="$platform_dir/$filename"
                    
                    if [[ ! -f "$dest" ]] || [[ "$file" -nt "$dest" ]]; then
                        cp "$file" "$dest"
                        print_package "Collected: $filename"
                        ((copied++))
                    else
                        print_info "Skipped (already exists): $filename"
                    fi
                fi
            done < <(find "$subdir_path" -type f \( -name "*.deb" -o -name "*.rpm" -o -name "*.tar.gz" -o -name "*.zip" -o -name "*.dmg" -o -name "*.pkg" -o -name "*.exe" -o -name "*.msi" \) -print0 2>/dev/null)
        fi
    done
    
    # Also look for packages directly in the release directory
    while IFS= read -r -d '' file; do
        if [[ -f "$file" ]]; then
            local filename=$(basename "$file")
            local dest="$platform_dir/$filename"
            
            if [[ ! -f "$dest" ]] || [[ "$file" -nt "$dest" ]]; then
                cp "$file" "$dest"
                print_package "Collected: $filename"
                ((copied++))
            else
                print_info "Skipped (already exists): $filename"
            fi
        fi
    done < <(find "$RELEASE_DIR" -maxdepth 1 -type f \( -name "*.deb" -o -name "*.rpm" -o -name "*.tar.gz" -o -name "*.zip" -o -name "*.dmg" -o -name "*.pkg" -o -name "*.exe" -o -name "*.msi" \) -print0 2>/dev/null)
    
    if [[ $copied -gt 0 ]]; then
        print_success "Collected $copied package(s) for $platform"
    else
        print_info "No new packages to collect for $platform"
    fi
    
    return $copied
}

create_platform_info() {
    local platform_dir="$1"
    local platform=$(detect_platform)
    local arch=$(detect_architecture)
    
    print_step "Creating platform information file..."
    
    local info_file="$platform_dir/PLATFORM_INFO.txt"
    cat > "$info_file" << EOF
Platform Information
====================

Platform: $platform
Architecture: $arch
Build Date: $(date '+%Y-%m-%d %H:%M:%S UTC')
Build System: $(uname -s) $(uname -r)
Build User: $(whoami)
Build Host: $(hostname)

Packages Collected:
EOF
    
    # List collected packages
    if [[ -d "$platform_dir" ]]; then
        while IFS= read -r -d '' file; do
            if [[ -f "$file" ]] && [[ "$(basename "$file")" != "PLATFORM_INFO.txt" ]]; then
                local filename=$(basename "$file")
                local size=$(du -h "$file" | cut -f1)
                local checksum=$(sha256sum "$file" | cut -d' ' -f1)
                
                echo "" >> "$info_file"
                echo "File: $filename" >> "$info_file"
                echo "Size: $size" >> "$info_file"
                echo "SHA256: $checksum" >> "$info_file"
            fi
        done < <(find "$platform_dir" -type f -print0 2>/dev/null)
    fi
    
    print_success "Platform info created: $info_file"
}

create_collection_summary() {
    local version_dir="$COLLECTION_DIR/v$VERSION"
    
    print_step "Creating collection summary..."
    
    local summary_file="$version_dir/COLLECTION_SUMMARY.md"
    cat > "$summary_file" << EOF
# Package Collection Summary v$VERSION

**Collection Date**: $(date '+%Y-%m-%d %H:%M:%S UTC')

## Platforms Collected

EOF
    
    # List all platforms with package counts
    if [[ -d "$version_dir" ]]; then
        for platform_dir in "$version_dir"/*/; do
            if [[ -d "$platform_dir" ]]; then
                local platform=$(basename "$platform_dir")
                local package_count=0
                
                # Count packages (excluding info files)
                while IFS= read -r -d '' file; do
                    if [[ -f "$file" ]] && [[ "$(basename "$file")" != "PLATFORM_INFO.txt" ]]; then
                        ((package_count++))
                    fi
                done < <(find "$platform_dir" -type f -print0 2>/dev/null)
                
                echo "### $platform" >> "$summary_file"
                echo "- **Packages**: $package_count" >> "$summary_file"
                echo "- **Directory**: \`$platform_dir\`" >> "$summary_file"
                echo "" >> "$summary_file"
            fi
        done
    fi
    
    cat >> "$summary_file" << EOF

## Next Steps

1. **Transfer packages** to your centralized release system
2. **Run centralized release script**: \`./scripts/centralized-release.sh --all\`
3. **Verify all packages** are included in the GitHub release

## Package Types Supported

- **Linux**: .deb, .rpm, .tar.gz
- **macOS**: .dmg, .pkg
- **Windows**: .exe, .msi
- **Source**: .tar.gz, .zip

## Directory Structure

\`\`\`
$COLLECTION_DIR/
└── v$VERSION/
    ├── COLLECTION_SUMMARY.md
    ├── linux-debian/
    ├── linux-redhat/
    ├── macos-x86_64/
    ├── macos-arm64/
    └── windows/
\`\`\`
EOF
    
    print_success "Collection summary created: $summary_file"
}

show_help() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -h, --help          Show this help message"
    echo "  -c, --collect       Collect packages from local build"
    echo "  -s, --summary       Create collection summary"
    echo "  -a, --all           Collect packages and create summary"
    echo "  -v, --version       Show version information"
    echo ""
    echo "Examples:"
    echo "  $0 --collect        # Collect packages from local build"
    echo "  $0 --summary        # Create collection summary"
    echo "  $0 --all            # Collect packages and create summary"
    echo ""
    echo "This script helps collect packages from different build systems."
    echo "Run this on each system that builds packages, then transfer"
    echo "the collection directory to your centralized release system."
}

main() {
    print_header
    
    # Parse command line arguments
    local do_collect=false
    local do_summary=false
    
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_help
                exit 0
                ;;
            -c|--collect)
                do_collect=true
                shift
                ;;
            -s|--summary)
                do_summary=true
                shift
                ;;
            -a|--all)
                do_collect=true
                do_summary=true
                shift
                ;;
            -v|--version)
                echo "ssmtp-mailer Package Collection Helper v$VERSION"
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
    if [[ "$do_collect" == false && "$do_summary" == false ]]; then
        show_help
        exit 0
    fi
    
    # Execute requested actions
    if [[ "$do_collect" == true ]]; then
        print_step "Creating package collection structure..."
        local platform_dir
        platform_dir=$(create_collection_structure)
        if [[ -n "$platform_dir" ]]; then
            collect_packages "$platform_dir"
            create_platform_info "$platform_dir"
        else
            print_error "Failed to create collection structure"
            exit 1
        fi
    fi
    
    if [[ "$do_summary" == true ]]; then
        create_collection_summary
    fi
    
    print_success "Package collection process completed!"
    echo ""
    print_info "Collection directory: $COLLECTION_DIR/v$VERSION/"
    print_info "Transfer this directory to your centralized release system"
    print_info "Then run: ./scripts/centralized-release.sh --all"
}

# Run main function with all arguments
main "$@"
