#!/bin/bash

# ssmtp-mailer Debian/Ubuntu Build Script
# This script automates the build process for Debian-based distributions

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Script variables
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"
DIST_DIR="$PROJECT_ROOT/dist"
VERSION="0.2.0"

# Build options
BUILD_TYPE="Release"
BUILD_ARCH=""
BUILD_TESTS="ON"
BUILD_EXAMPLES="OFF"
ENABLE_LOGGING="ON"
ENABLE_SSL="ON"
CLEAN_BUILD=false
VERBOSE=false
PARALLEL_JOBS=$(nproc)

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

Build ssmtp-mailer for different architectures and configurations.

OPTIONS:
    -h, --help              Show this help message
    -v, --version           Show version information
    -c, --clean             Clean build directory before building
    -d, --debug             Build in debug mode
    -r, --release           Build in release mode (default)
    -a, --arch ARCH         Build for specific architecture (32, 64, both)
    -j, --jobs N            Number of parallel jobs (default: auto-detect)
    --no-tests              Disable building tests
    --examples              Enable building examples
    --no-logging            Disable logging
    --no-ssl                Disable SSL support
    --verbose               Enable verbose output
    --package               Build DEB package after successful build

EXAMPLES:
    $0                        # Build release version for current architecture
    $0 -d                    # Build debug version
    $0 -a both               # Build for both 32-bit and 64-bit
    $0 -c -r                 # Clean build and build release version
    $0 --package             # Build and create DEB package

EOF
}

# Function to show version
show_version() {
    echo "ssmtp-mailer build script v$VERSION"
}

# Function to detect Debian distribution
detect_distribution() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        DISTRO_NAME="$NAME"
        DISTRO_VERSION="$VERSION_ID"
        DISTRO_CODENAME="$VERSION_CODENAME"
    elif [ -f /etc/debian_version ]; then
        DISTRO_NAME="Debian"
        DISTRO_VERSION=$(cat /etc/debian_version)
        DISTRO_CODENAME="unknown"
    else
        DISTRO_NAME="Unknown"
        DISTRO_VERSION="unknown"
        DISTRO_CODENAME="unknown"
    fi
    
    print_status "Detected distribution: $DISTRO_NAME $DISTRO_VERSION ($DISTRO_CODENAME)"
}

# Function to check and install dependencies
check_dependencies() {
    print_status "Checking and installing Debian/Ubuntu dependencies..."
    
    # Update package list
    sudo apt-get update
    
    # Install essential build tools
    local build_packages=(
        "build-essential"
        "cmake"
        "pkg-config"
        "git"
        "wget"
        "curl"
    )
    
    # Install SSL development libraries
    local ssl_packages=(
        "libssl-dev"
        "libcrypto++-dev"
    )
    
    # Install JSON and HTTP libraries
    local lib_packages=(
        "libjsoncpp-dev"
        "libcurl4-openssl-dev"
    )
    
    # Install package building tools
    local package_packages=(
        "fakeroot"
        "devscripts"
        "debhelper"
        "dh-make"
        "lintian"
    )
    
    # Install all packages
    print_status "Installing build tools..."
    sudo apt-get install -y "${build_packages[@]}"
    
    print_status "Installing SSL libraries..."
    sudo apt-get install -y "${ssl_packages[@]}"
    
    print_status "Installing JSON and HTTP libraries..."
    sudo apt-get install -y "${lib_packages[@]}"
    
    if [ "$CREATE_PACKAGES" = true ]; then
        print_status "Installing package building tools..."
        sudo apt-get install -y "${package_packages[@]}"
    fi
    
    print_success "All dependencies installed successfully"
}

# Function to clean build directory
clean_build() {
    if [ "$CLEAN_BUILD" = true ]; then
        print_status "Cleaning build directory..."
        rm -rf "$BUILD_DIR"
        print_success "Build directory cleaned"
    fi
}

# Function to create build directory
create_build_dir() {
    print_status "Creating build directory..."
    mkdir -p "$BUILD_DIR"
    print_success "Build directory created"
}

# Function to configure build
configure_build() {
    local arch_flag=""
    local build_type="$BUILD_TYPE"
    
    if [ -n "$BUILD_ARCH" ]; then
        case "$BUILD_ARCH" in
            32)
                arch_flag="-DCMAKE_CXX_FLAGS=-m32 -DCMAKE_LD_FLAGS=-m32"
                ;;
            64)
                arch_flag="-DCMAKE_CXX_FLAGS=-m64 -DCMAKE_LD_FLAGS=-m64"
                ;;
            both)
                # This will be handled separately
                return 0
                ;;
            *)
                print_error "Invalid architecture: $BUILD_ARCH"
                exit 1
                ;;
        esac
    fi
    
    print_status "Configuring build (${build_type})..."
    
    cd "$BUILD_DIR"
    
    local cmake_cmd="cmake .."
    cmake_cmd="$cmake_cmd -DCMAKE_BUILD_TYPE=$build_type"
    cmake_cmd="$cmake_cmd -DBUILD_TESTS=$BUILD_TESTS"
    cmake_cmd="$cmake_cmd -DBUILD_EXAMPLES=$BUILD_EXAMPLES"
    cmake_cmd="$cmake_cmd -DENABLE_LOGGING=$ENABLE_LOGGING"
    cmake_cmd="$cmake_cmd -DENABLE_SSL=$ENABLE_SSL"
    
    if [ -n "$arch_flag" ]; then
        cmake_cmd="$cmake_cmd $arch_flag"
    fi
    
    if [ "$VERBOSE" = true ]; then
        cmake_cmd="$cmake_cmd --verbose"
    fi
    
    print_status "Running: $cmake_cmd"
    
    if ! eval $cmake_cmd; then
        print_error "CMake configuration failed"
        exit 1
    fi
    
    print_success "Build configured successfully"
}

# Function to build project
build_project() {
    print_status "Building project..."
    
    cd "$BUILD_DIR"
    
    local make_cmd="make -j$PARALLEL_JOBS"
    
    if [ "$VERBOSE" = true ]; then
        make_cmd="$make_cmd VERBOSE=1"
    fi
    
    print_status "Running: $make_cmd"
    
    if ! eval $make_cmd; then
        print_error "Build failed"
        exit 1
    fi
    
    print_success "Build completed successfully"
}

# Function to build for both architectures
build_both_architectures() {
    print_status "Building for both 32-bit and 64-bit architectures..."
    
    # Build 32-bit version
    print_status "Building 32-bit version..."
    BUILD_ARCH="32"
    configure_build
    build_project
    
    # Clean and build 64-bit version
    print_status "Building 64-bit version..."
    rm -rf "$BUILD_DIR"
    BUILD_ARCH="64"
    configure_build
    build_project
    
    print_success "Both architectures built successfully"
}

# Function to run tests
run_tests() {
    if [ "$BUILD_TESTS" = "ON" ]; then
        print_status "Running tests..."
        cd "$BUILD_DIR"
        
        if ! make test; then
            print_warning "Some tests failed"
        else
            print_success "All tests passed"
        fi
    fi
}

# Function to create DEB package
create_deb_package() {
    if [ "$CREATE_PACKAGES" = true ]; then
        print_status "Creating DEB package..."
        cd "$BUILD_DIR"
        
        # Create DEB package using CPack
        if command -v cpack &> /dev/null; then
            if cpack -G DEB; then
                print_success "DEB package created"
                
                # Move package to dist directory
                if [ -d "$DIST_DIR" ]; then
                    print_status "Moving DEB package to dist directory..."
                    mv *.deb "$DIST_DIR/" 2>/dev/null || true
                    print_success "DEB package moved to dist directory"
                fi
            else
                print_warning "DEB package creation failed"
            fi
        else
            print_warning "cpack not found, skipping DEB package creation"
        fi
    fi
}

# Function to show build summary
show_build_summary() {
    print_success "Build completed successfully!"
    echo
    echo "Build Summary:"
    echo "  Distribution: $DISTRO_NAME $DISTRO_VERSION"
    echo "  Build Type: $BUILD_TYPE"
    echo "  Architecture: ${BUILD_ARCH:-auto-detect}"
    echo "  Tests: $BUILD_TESTS"
    echo "  Examples: $BUILD_EXAMPLES"
    echo "  Logging: $ENABLE_LOGGING"
    echo "  SSL: $ENABLE_SSL"
    echo "  Build Directory: $BUILD_DIR"
    echo "  Distribution Directory: $DIST_DIR"
    echo
    echo "Next steps:"
    echo "  - Run tests: cd $BUILD_DIR && make test"
    echo "  - Install: cd $BUILD_DIR && sudo make install"
    if [ "$CREATE_PACKAGES" = true ]; then
        echo "  - Install DEB package: sudo dpkg -i $DIST_DIR/*.deb"
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
        -c|--clean)
            CLEAN_BUILD=true
            shift
            ;;
        -d|--debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        -r|--release)
            BUILD_TYPE="Release"
            shift
            ;;
        -a|--arch)
            BUILD_ARCH="$2"
            shift 2
            ;;
        -j|--jobs)
            PARALLEL_JOBS="$2"
            shift 2
            ;;
        --no-tests)
            BUILD_TESTS="OFF"
            shift
            ;;
        --examples)
            BUILD_EXAMPLES="ON"
            shift
            ;;
        --no-logging)
            ENABLE_LOGGING="OFF"
            shift
            ;;
        --no-ssl)
            ENABLE_SSL="OFF"
            shift
            ;;
        --verbose)
            VERBOSE=true
            shift
            ;;
        --package)
            CREATE_PACKAGES=true
            shift
            ;;
        *)
            print_error "Unknown option: $1"
            show_usage
            exit 1
            ;;
    esac
done

# Main build process
main() {
    print_status "Starting ssmtp-mailer build process..."
    print_status "Project root: $PROJECT_ROOT"
    
    # Detect distribution
    detect_distribution
    
    # Check and install dependencies
    check_dependencies
    
    # Clean build if requested
    clean_build
    
    # Create build directory
    create_build_dir
    
    # Configure and build
    if [ "$BUILD_ARCH" = "both" ]; then
        build_both_architectures
    else
        configure_build
        build_project
    fi
    
    # Run tests
    run_tests
    
    # Create DEB package if requested
    create_deb_package
    
    # Show build summary
    show_build_summary
}

# Run main function
main "$@"
