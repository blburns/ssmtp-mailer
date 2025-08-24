# Makefile for ssmtp-mailer
# Simple SMTP Mailer - A smart host MTA for multiple domains

# Variables
PROJECT_NAME = ssmtp-mailer
VERSION = 1.0.0
BUILD_DIR = build
DIST_DIR = dist
PACKAGE_DIR = packaging

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2 -DNDEBUG
LDFLAGS = -lssl -lcrypto -lpthread

# Directories
SRC_DIR = src
INCLUDE_DIR = include
CONFIG_DIR = config
SCRIPTS_DIR = scripts

# Default target
all: build

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build using CMake
build: $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. && make -j$(shell nproc)

# Clean build
clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(DIST_DIR)

# Install
install: build
	cd $(BUILD_DIR) && sudo make install

# Uninstall
uninstall:
	sudo rm -f /usr/local/bin/$(PROJECT_NAME)
	sudo rm -f /usr/local/lib/lib$(PROJECT_NAME).so
	sudo rm -rf /usr/local/include/$(PROJECT_NAME)
	sudo rm -rf /etc/$(PROJECT_NAME)

# Test
test: build
	cd $(BUILD_DIR) && make test

# Package RPM
package-rpm: build
	@echo "Building RPM package..."
	@mkdir -p $(DIST_DIR)
	cd $(BUILD_DIR) && cpack -G RPM
	mv $(BUILD_DIR)/$(PROJECT_NAME)-$(VERSION)-*.rpm $(DIST_DIR)/

# Package DEB
package-deb: build
	@echo "Building DEB package..."
	@mkdir -p $(DIST_DIR)
	cd $(BUILD_DIR) && cpack -G DEB
	mv $(BUILD_DIR)/$(PROJECT_NAME)-$(VERSION)-*.deb $(DIST_DIR)/

# Package all
package: package-rpm package-deb

# Build 32-bit version
build-32: $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake -DCMAKE_CXX_FLAGS="-m32" -DCMAKE_LD_FLAGS="-m32" .. && make -j$(shell nproc)

# Build 64-bit version
build-64: $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake -DCMAKE_CXX_FLAGS="-m64" -DCMAKE_LD_FLAGS="-m64" .. && make -j$(shell nproc)

# Build both architectures
build-multiarch: build-32 build-64

# Development build with debug info
debug: $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=Debug .. && make -j$(shell nproc)

# Release build
release: $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j$(shell nproc)

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
	sudo apt-get update
	sudo apt-get install -y build-essential cmake libssl-dev pkg-config
	# For RPM building
	sudo apt-get install -y rpm rpm-build
	# For DEB building
	sudo apt-get install -y fakeroot devscripts

# Docker build
docker-build:
	docker build -t $(PROJECT_NAME):$(VERSION) .

# Docker run
docker-run:
	docker run -it --rm $(PROJECT_NAME):$(VERSION)

# Help
help:
	@echo "Available targets:"
	@echo "  all          - Build the project (default)"
	@echo "  build        - Build using CMake"
	@echo "  clean        - Clean build artifacts"
	@echo "  install      - Install to system"
	@echo "  uninstall    - Remove from system"
	@echo "  test         - Run tests"
	@echo "  package      - Build RPM and DEB packages"
	@echo "  package-rpm  - Build RPM package only"
	@echo "  package-deb  - Build DEB package only"
	@echo "  build-32     - Build 32-bit version"
	@echo "  build-64     - Build 64-bit version"
	@echo "  debug        - Build with debug info"
	@echo "  release      - Build release version"
	@echo "  analyze      - Run static analysis"
	@echo "  format       - Format code"
	@echo "  check-style  - Check code style"
	@echo "  deps         - Install dependencies"
	@echo "  docker-build - Build Docker image"
	@echo "  docker-run   - Run Docker container"
	@echo "  help         - Show this help"

.PHONY: all build clean install uninstall test package package-rpm package-deb build-32 build-64 build-multiarch debug release analyze format check-style deps docker-build docker-run help
