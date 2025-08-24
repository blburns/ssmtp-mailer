# Makefile for ssmtp-mailer
# Simple SMTP Mailer - A smart host MTA for multiple domains

# Variables
PROJECT_NAME = ssmtp-mailer
VERSION = 0.1.0
BUILD_DIR = build
DIST_DIR = dist
PACKAGE_DIR = packaging

# Platform detection
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    PLATFORM = macos
    CXX = clang++
    CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2 -DNDEBUG
    LDFLAGS = -lssl -lcrypto
    # macOS specific flags
    CXXFLAGS += -target x86_64-apple-macos11.0 -target arm64-apple-macos11.0
    # Detect processor cores for parallel builds
    PARALLEL_JOBS = $(shell sysctl -n hw.ncpu)
    # macOS install paths
    INSTALL_PREFIX = /usr/local
    CONFIG_DIR = $(INSTALL_PREFIX)/etc/$(PROJECT_NAME)
else
    PLATFORM = linux
    CXX = g++
    CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2 -DNDEBUG
    LDFLAGS = -lssl -lcrypto -lpthread
    # Linux specific flags
    PARALLEL_JOBS = $(shell nproc)
    # Linux install paths
    INSTALL_PREFIX = /usr/local
    CONFIG_DIR = /etc/$(PROJECT_NAME)
endif

# Directories
SRC_DIR = src
INCLUDE_DIR = include
CONFIG_DIR_SRC = config
SCRIPTS_DIR = scripts

# Default target
all: build

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build using CMake
build: $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. && make -j$(PARALLEL_JOBS)

# Clean build
clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(DIST_DIR)

# Install
install: build
	cd $(BUILD_DIR) && sudo make install

# Uninstall
uninstall:
	sudo rm -f $(INSTALL_PREFIX)/bin/$(PROJECT_NAME)
	sudo rm -f $(INSTALL_PREFIX)/lib/lib$(PROJECT_NAME).so
	sudo rm -f $(INSTALL_PREFIX)/lib/lib$(PROJECT_NAME).dylib
	sudo rm -rf $(INSTALL_PREFIX)/include/$(PROJECT_NAME)
	sudo rm -rf $(CONFIG_DIR)

# Test
test: build
	cd $(BUILD_DIR) && make test

# Package RPM (Linux only)
package-rpm: build
ifeq ($(PLATFORM),linux)
	@echo "Building RPM package..."
	@mkdir -p $(DIST_DIR)
	cd $(BUILD_DIR) && cpack -G RPM
	mv $(BUILD_DIR)/$(PROJECT_NAME)-$(VERSION)-*.rpm $(DIST_DIR)/
else
	@echo "RPM packages are only supported on Linux"
endif

# Package DEB (Linux only)
package-deb: build
ifeq ($(PLATFORM),linux)
	@echo "Building DEB package..."
	@mkdir -p $(DIST_DIR)
	cd $(BUILD_DIR) && cpack -G DEB
	mv $(BUILD_DIR)/$(PROJECT_NAME)-$(VERSION)-*.deb $(DIST_DIR)/
else
	@echo "DEB packages are only supported on Linux"
endif

# Package DMG (macOS only)
package-dmg: build
ifeq ($(PLATFORM),macos)
	@echo "Building macOS DMG package..."
	@mkdir -p $(DIST_DIR)
	cd $(BUILD_DIR) && cpack -G DragNDrop
	mv $(BUILD_DIR)/$(PROJECT_NAME)-$(VERSION)-*.dmg $(DIST_DIR)/
else
	@echo "DMG packages are only supported on macOS"
endif

# Package all (platform-specific)
package: 
ifeq ($(PLATFORM),macos)
	$(MAKE) package-dmg
else
	$(MAKE) package-rpm package-deb
endif

# Build 32-bit version (Linux only)
build-32: $(BUILD_DIR)
ifeq ($(PLATFORM),linux)
	cd $(BUILD_DIR) && cmake -DCMAKE_CXX_FLAGS="-m32" -DCMAKE_LD_FLAGS="-m32" .. && make -j$(PARALLEL_JOBS)
else
	@echo "32-bit builds are only supported on Linux"
endif

# Build 64-bit version (Linux only)
build-64: $(BUILD_DIR)
ifeq ($(PLATFORM),linux)
	cd $(BUILD_DIR) && cmake -DCMAKE_CXX_FLAGS="-m64" -DCMAKE_LD_FLAGS="-m64" .. && make -j$(PARALLEL_JOBS)
else
	@echo "64-bit builds are only supported on Linux"
endif

# Build both architectures (Linux only)
build-multiarch: build-32 build-64

# Build universal binary (macOS only)
build-universal: $(BUILD_DIR)
ifeq ($(PLATFORM),macos)
	cd $(BUILD_DIR) && cmake -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" .. && make -j$(PARALLEL_JOBS)
else
	@echo "Universal binary builds are only supported on macOS"
endif

# Development build with debug info
debug: $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=Debug .. && make -j$(PARALLEL_JOBS)

# Release build
release: $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j$(PARALLEL_JOBS)

# Static analysis
analyze: $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake -DCMAKE_CXX_CLANG_TIDY=clang-tidy .. && make

# Format code
format:
	find $(SRC_DIR) $(INCLUDE_DIR) -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i

# Check code style
check-style:
	find $(SRC_DIR) $(INCLUDE_DIR) -name "*.cpp" -o -name "*.hpp" | xargs clang-format --dry-run --Werror

# Dependencies
deps:
ifeq ($(PLATFORM),macos)
	@echo "Installing macOS dependencies..."
	# Check if Homebrew is installed
	@if ! command -v brew &> /dev/null; then \
		echo "Installing Homebrew..."; \
		/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"; \
	fi
	brew install cmake openssl@3 pkg-config
	# Install Xcode command line tools if not present
	@if ! xcode-select -p &> /dev/null; then \
		echo "Installing Xcode command line tools..."; \
		xcode-select --install; \
	fi
else
	@echo "Installing Linux dependencies..."
	sudo apt-get update
	sudo apt-get install -y build-essential cmake libssl-dev pkg-config
	# For RPM building
	sudo apt-get install -y rpm rpm-build
	# For DEB building
	sudo apt-get install -y fakeroot devscripts
endif

# Docker build (Linux only)
docker-build:
ifeq ($(PLATFORM),linux)
	docker build -t $(PROJECT_NAME):$(VERSION) .
else
	@echo "Docker builds are only supported on Linux"
endif

# Docker run (Linux only)
docker-run:
ifeq ($(PLATFORM),linux)
	docker run -it --rm $(PROJECT_NAME):$(VERSION)
else
	@echo "Docker is only supported on Linux"
endif

# macOS specific: Build for Intel only
build-intel: $(BUILD_DIR)
ifeq ($(PLATFORM),macos)
	cd $(BUILD_DIR) && cmake -DCMAKE_OSX_ARCHITECTURES="x86_64" .. && make -j$(PARALLEL_JOBS)
else
	@echo "Intel builds are only supported on macOS"
endif

# macOS specific: Build for Apple Silicon only
build-arm64: $(BUILD_DIR)
ifeq ($(PLATFORM),macos)
	cd $(BUILD_DIR) && cmake -DCMAKE_OSX_ARCHITECTURES="arm64" .. && make -j$(PARALLEL_JOBS)
else
	@echo "ARM64 builds are only supported on macOS"
endif

# Help
help:
	@echo "Available targets:"
	@echo "  all          - Build the project (default)"
	@echo "  build        - Build using CMake"
	@echo "  clean        - Clean build artifacts"
	@echo "  install      - Install to system"
	@echo "  uninstall    - Remove from system"
	@echo "  test         - Run tests"
	@echo "  package      - Build platform-specific packages"
ifeq ($(PLATFORM),macos)
	@echo "  package-dmg  - Build macOS DMG package"
	@echo "  build-universal - Build universal binary (Intel + Apple Silicon)"
	@echo "  build-intel  - Build for Intel Macs only"
	@echo "  build-arm64  - Build for Apple Silicon Macs only"
else
	@echo "  package-rpm  - Build RPM package only"
	@echo "  package-deb  - Build DEB package only"
	@echo "  build-32     - Build 32-bit version"
	@echo "  build-64     - Build 64-bit version"
	@echo "  build-multiarch - Build for both 32-bit and 64-bit"
endif
	@echo "  debug        - Build with debug info"
	@echo "  release      - Build release version"
	@echo "  analyze      - Run static analysis"
	@echo "  format       - Format code"
	@echo "  check-style  - Check code style"
	@echo "  deps         - Install dependencies"
ifeq ($(PLATFORM),linux)
	@echo "  docker-build - Build Docker image"
	@echo "  docker-run   - Run Docker container"
endif
	@echo "  help         - Show this help"
	@echo ""
	@echo "Platform: $(PLATFORM)"
	@echo "Compiler: $(CXX)"
	@echo "Parallel jobs: $(PARALLEL_JOBS)"

.PHONY: all build clean install uninstall test package package-rpm package-deb package-dmg build-32 build-64 build-multiarch build-universal build-intel build-arm64 debug release analyze format check-style deps docker-build docker-run help
