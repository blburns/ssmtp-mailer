#!/bin/bash

# ssmtp-mailer Build Script
# This script automates the build process for different architectures and configurations

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
VERSION="0.1.0"

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
    --package               Build packages after successful build

EXAMPLES:
    $0                        # Build release version for current architecture
    $0 -d                    # Build debug version
    $0 -a both               # Build for both 32-bit and 64-bit
    $0 -c -r                 # Clean build and build release version
    $0 --package             # Build and create packages

EOF
}

# Function to show version
show_version() {
    echo "ssmtp-mailer build script v$VERSION"
}

# Function to check dependencies
check_dependencies() {
    print_status "Checking build dependencies..."
    
    local missing_deps=()
    
    # Check for required tools
    for tool in cmake make g++ pkg-config; do
        if ! command -v $tool &> /dev/null; then
            missing_deps+=("$tool")
        fi
    done
    
    # Check for OpenSSL development libraries
    if ! pkg-config --exists openssl; then
        missing_deps+=("libssl-dev")
    fi
    
    if [ ${#missing_deps[@]} -ne 0 ]; then
        print_error "Missing dependencies: ${missing_deps[*]}"
        print_status "Install them using: sudo apt-get install ${missing_deps[*]}"
        exit 1
    fi
    
    print_success "All dependencies satisfied"
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

# Function to create packages
create_packages() {
    print_status "Creating packages..."
    cd "$BUILD_DIR"
    
    # Create RPM package
    if command -v cpack &> /dev/null; then
        print_status "Creating RPM package..."
        if cpack -G RPM; then
            print_success "RPM package created"
        else
            print_warning "RPM package creation failed"
        fi
    else
        print_warning "cpack not found, skipping RPM package creation"
    fi
    
    # Create DEB package
    if command -v cpack &> /dev/null; then
        print_status "Creating DEB package..."
        if cpack -G DEB; then
            print_success "DEB package created"
        else
            print_warning "DEB package creation failed"
        fi
    else
        print_warning "cpack not found, skipping DEB package creation"
    fi
    
    # Move packages to dist directory
    if [ -d "$DIST_DIR" ]; then
        print_status "Moving packages to dist directory..."
        mv *.rpm *.deb "$DIST_DIR/" 2>/dev/null || true
        print_success "Packages moved to dist directory"
    fi
}

# Function to show build summary
show_build_summary() {
    print_success "Build completed successfully!"
    echo
    echo "Build Summary:"
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
    echo "  - Create packages: cd $BUILD_DIR && cpack"
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
    
    # Check dependencies
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
    
    # Create packages if requested
    if [ "$CREATE_PACKAGES" = true ]; then
        create_packages
    fi
    
    # Show build summary
    show_build_summary
}

# Run main function
main "$@"
