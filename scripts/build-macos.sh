#!/bin/bash

# ssmtp-mailer macOS Build Script
# This script automates the build process for macOS Big Sur 11.0+ and later

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
BUILD_ARCH="universal"
BUILD_TESTS="ON"
BUILD_EXAMPLES="OFF"
ENABLE_LOGGING="ON"
ENABLE_SSL="ON"
CLEAN_BUILD=false
VERBOSE=false
PARALLEL_JOBS=$(sysctl -n hw.ncpu)

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

Build ssmtp-mailer for macOS Big Sur 11.0+ and later.

OPTIONS:
    -h, --help              Show this help message
    -v, --version           Show version information
    -c, --clean             Clean build directory before building
    -d, --debug             Build in debug mode
    -r, --release           Build in release mode (default)
    -a, --arch ARCH         Build for specific architecture (universal, intel, arm64)
    -j, --jobs N            Number of parallel jobs (default: auto-detect)
    --no-tests              Disable building tests
    --examples              Enable building examples
    --no-logging            Disable logging
    --no-ssl                Disable SSL support
    --verbose               Enable verbose output
    --package               Build DMG package after successful build

EXAMPLES:
    $0                        # Build release version for universal binary
    $0 -d                    # Build debug version
    $0 -a intel              # Build for Intel Macs only
    $0 -a arm64              # Build for Apple Silicon Macs only
    $0 -c -r                 # Clean build and build release version
    $0 --package             # Build and create DMG package

EOF
}

# Function to show version
show_version() {
    echo "ssmtp-mailer macOS build script v$VERSION"
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

# Function to check dependencies
check_dependencies() {
    print_status "Checking macOS build dependencies..."
    
    local missing_deps=()
    
    # Check for Xcode command line tools
    if ! xcode-select -p &> /dev/null; then
        print_warning "Xcode command line tools not found"
        print_status "Installing Xcode command line tools..."
        xcode-select --install
        print_status "Please complete the Xcode installation and run this script again"
        exit 1
    fi
    
    # Check for required tools
    for tool in cmake make pkg-config; do
        if ! command -v $tool &> /dev/null; then
            missing_deps+=("$tool")
        fi
    done
    
    # Check for OpenSSL
    if ! pkg-config --exists openssl; then
        missing_deps+=("openssl")
    fi
    
    if [ ${#missing_deps[@]} -ne 0 ]; then
        print_warning "Missing dependencies: ${missing_deps[*]}"
        print_status "Installing missing dependencies via Homebrew..."
        install_dependencies
    else
        print_success "All dependencies are available"
    fi
}

# Function to install dependencies
install_dependencies() {
    # Check if Homebrew is installed
    if ! command -v brew &> /dev/null; then
        print_status "Installing Homebrew..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
        
        # Add Homebrew to PATH for this session
        if [[ "$(uname -m)" == "arm64" ]]; then
            eval "$(/opt/homebrew/bin/brew shellenv)"
        else
            eval "$(/usr/local/bin/brew shellenv)"
        fi
    fi
    
    # Install required packages
    print_status "Installing packages via Homebrew..."
    brew install cmake openssl@3 pkg-config
    
    print_success "Dependencies installed successfully"
}

# Function to detect architecture
detect_architecture() {
    local arch=$(uname -m)
    if [ "$arch" == "arm64" ]; then
        echo "arm64"
    elif [ "$arch" == "x86_64" ]; then
        echo "x86_64"
    else
        echo "unknown"
    fi
}

# Function to set build architecture
set_build_architecture() {
    case $BUILD_ARCH in
        "universal")
            CMAKE_ARCH_FLAGS="-DCMAKE_OSX_ARCHITECTURES=x86_64;arm64"
            print_status "Building universal binary (Intel + Apple Silicon)"
            ;;
        "intel")
            CMAKE_ARCH_FLAGS="-DCMAKE_OSX_ARCHITECTURES=x86_64"
            print_status "Building for Intel Macs only"
            ;;
        "arm64")
            CMAKE_ARCH_FLAGS="-DCMAKE_OSX_ARCHITECTURES=arm64"
            print_status "Building for Apple Silicon Macs only"
            ;;
        *)
            print_error "Invalid architecture: $BUILD_ARCH"
            print_error "Valid options: universal, intel, arm64"
            exit 1
            ;;
    esac
}

# Function to create build directory
create_build_directory() {
    if [ "$CLEAN_BUILD" = true ]; then
        print_status "Cleaning build directory..."
        rm -rf "$BUILD_DIR"
    fi
    
    mkdir -p "$BUILD_DIR"
    print_success "Build directory ready: $BUILD_DIR"
}

# Function to configure CMake
configure_cmake() {
    print_status "Configuring CMake..."
    
    local cmake_flags=(
        "-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
        "-DBUILD_TESTS=$BUILD_TESTS"
        "-DBUILD_EXAMPLES=$BUILD_EXAMPLES"
        "-DENABLE_LOGGING=$ENABLE_LOGGING"
        "-DENABLE_SSL=$ENABLE_SSL"
        "-DCMAKE_OSX_DEPLOYMENT_TARGET=11.0"
        $CMAKE_ARCH_FLAGS
    )
    
    if [ "$VERBOSE" = true ]; then
        cmake_flags+=("--verbose")
    fi
    
    cd "$BUILD_DIR"
    cmake "${cmake_flags[@]}" "$PROJECT_ROOT"
    
    if [ $? -eq 0 ]; then
        print_success "CMake configuration completed"
    else
        print_error "CMake configuration failed"
        exit 1
    fi
}

# Function to build project
build_project() {
    print_status "Building project with $PARALLEL_JOBS parallel jobs..."
    
    cd "$BUILD_DIR"
    make -j"$PARALLEL_JOBS"
    
    if [ $? -eq 0 ]; then
        print_success "Build completed successfully"
    else
        print_error "Build failed"
        exit 1
    fi
}

# Function to run tests
run_tests() {
    if [ "$BUILD_TESTS" = "ON" ]; then
        print_status "Running tests..."
        cd "$BUILD_DIR"
        make test
        
        if [ $? -eq 0 ]; then
            print_success "All tests passed"
        else
            print_warning "Some tests failed"
        fi
    fi
}

# Function to create package
create_package() {
    if [ "$1" = "--package" ]; then
        print_status "Creating macOS DMG package..."
        cd "$BUILD_DIR"
        
        # Create distribution directory
        mkdir -p "$DIST_DIR"
        
        # Generate package
        cpack -G DragNDrop
        
        # Move package to dist directory
        if ls *.dmg 1> /dev/null 2>&1; then
            mv *.dmg "$DIST_DIR/"
            print_success "DMG package created: $DIST_DIR/"
        else
            print_warning "No DMG package found"
        fi
    fi
}

# Function to show build summary
show_build_summary() {
    print_success "Build completed successfully!"
    echo ""
    echo "Build Summary:"
    echo "  Platform: macOS"
    echo "  Architecture: $BUILD_ARCH"
    echo "  Build Type: $BUILD_TYPE"
    echo "  Build Directory: $BUILD_DIR"
    echo "  Tests: $BUILD_TESTS"
    echo "  Examples: $BUILD_EXAMPLES"
    echo "  Logging: $ENABLE_LOGGING"
    echo "  SSL: $ENABLE_SSL"
    echo ""
    
    if [ -f "$BUILD_DIR/bin/ssmtp-mailer" ]; then
        echo "Executable: $BUILD_DIR/bin/ssmtp-mailer"
        echo "Library: $BUILD_DIR/lib/libssmtp-mailer.dylib"
    fi
    
    if [ -d "$DIST_DIR" ] && ls "$DIST_DIR"/*.dmg 1> /dev/null 2>&1; then
        echo ""
        echo "Packages created in: $DIST_DIR/"
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
            PACKAGE_REQUESTED=true
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
    print_status "Starting ssmtp-mailer macOS build..."
    print_status "Project: $PROJECT_ROOT"
    print_status "Build directory: $BUILD_DIR"
    print_status "Distribution directory: $DIST_DIR"
    
    # Check macOS version
    check_macos_version
    
    # Check dependencies
    check_dependencies
    
    # Set build architecture
    set_build_architecture
    
    # Create build directory
    create_build_directory
    
    # Configure CMake
    configure_cmake
    
    # Build project
    build_project
    
    # Run tests
    run_tests
    
    # Create package if requested
    if [ "$PACKAGE_REQUESTED" = true ]; then
        create_package --package
    fi
    
    # Show build summary
    show_build_summary
}

# Run main function
main "$@"
