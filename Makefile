# Makefile for simple-smtp-mailer
# Simple SMTP Mailer - A smart host MTA for multiple domains

# Variables
PROJECT_NAME = simple-smtp-mailer
VERSION = 0.2.0
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
$(BUILD_DIR)-dir:
	mkdir -p $(BUILD_DIR)

# Build using CMake
build: $(BUILD_DIR)-dir
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

# Package using build scripts (recommended)
package-script:
ifeq ($(PLATFORM),macos)
	@echo "Building macOS package with build script..."
	@mkdir -p $(DIST_DIR)
	./scripts/build-macos.sh
	@echo "macOS package created successfully!"
else ifeq ($(PLATFORM),linux)
	@echo "Building Linux package with build script..."
	@mkdir -p $(DIST_DIR)
	./scripts/build-linux.sh
	@echo "Linux package created successfully!"
else
	@echo "Package script not available for this platform"
endif

# Package all platforms (creates organized release structure)
package-all: clean
	@echo "Building packages for all supported platforms..."
	@mkdir -p $(DIST_DIR)/releases/v$(VERSION)
	@mkdir -p $(DIST_DIR)/releases/v$(VERSION)/linux
	@mkdir -p $(DIST_DIR)/releases/v$(VERSION)/macos
	@mkdir -p $(DIST_DIR)/releases/v$(VERSION)/windows
	@mkdir -p $(DIST_DIR)/releases/v$(VERSION)/source
	@echo "Created release directory structure: $(DIST_DIR)/releases/v$(VERSION)/"
	
	@echo "Building Linux packages..."
	@if [ "$(PLATFORM)" = "linux" ]; then \
		$(MAKE) package-linux-release; \
	else \
		echo "Skipping Linux packages (not on Linux platform)"; \
	fi
	
	@echo "Building macOS packages..."
	@if [ "$(PLATFORM)" = "macos" ]; then \
		$(MAKE) package-macos-release; \
	else \
		echo "Skipping macOS packages (not on macOS platform)"; \
	fi
	
	@echo "Building source packages..."
	$(MAKE) package-source-release
	
	@echo "Creating release summary..."
	@echo "Release v$(VERSION) packages created in: $(DIST_DIR)/releases/v$(VERSION)/"
	@echo "Contents:"
	@ls -la $(DIST_DIR)/releases/v$(VERSION)/*/ 2>/dev/null || echo "No packages created yet"

# Package Linux release (organized)
package-linux-release: build
	@echo "Building organized Linux release packages..."
	@mkdir -p $(DIST_DIR)/releases/v$(VERSION)/linux
	cd $(BUILD_DIR) && cpack -G DEB
	cd $(BUILD_DIR) && cpack -G RPM
	cd $(BUILD_DIR) && cpack -G TGZ
	@if ls $(BUILD_DIR)/$(PROJECT_NAME)-$(VERSION)-*.deb 1> /dev/null 2>&1; then \
		mv $(BUILD_DIR)/$(PROJECT_NAME)-$(VERSION)-*.deb $(DIST_DIR)/releases/v$(VERSION)/linux/; \
		echo "DEB packages moved to: $(DIST_DIR)/releases/v$(VERSION)/linux/"; \
	fi
	@if ls $(BUILD_DIR)/$(PROJECT_NAME)-$(VERSION)-*.rpm 1> /dev/null 2>&1; then \
		mv $(BUILD_DIR)/$(PROJECT_NAME)-$(VERSION)-*.rpm $(DIST_DIR)/releases/v$(VERSION)/linux/; \
		echo "RPM packages moved to: $(DIST_DIR)/releases/v$(VERSION)/linux/"; \
	fi
	@if ls $(BUILD_DIR)/$(PROJECT_NAME)-$(VERSION)-*.tar.gz 1> /dev/null 2>&1; then \
		mv $(BUILD_DIR)/$(PROJECT_NAME)-$(VERSION)-*.tar.gz $(DIST_DIR)/releases/v$(VERSION)/linux/; \
		echo "TGZ packages moved to: $(DIST_DIR)/releases/v$(VERSION)/linux/"; \
	fi

# Package macOS release (organized)
package-macos-release:
	@echo "Building organized macOS release packages..."
	@mkdir -p $(DIST_DIR)/releases/v$(VERSION)/macos
	./scripts/build-macos.sh
	@if ls $(DIST_DIR)/$(PROJECT_NAME)-$(VERSION)-*.dmg 1> /dev/null 2>&1; then \
		mv $(DIST_DIR)/$(PROJECT_NAME)-$(VERSION)-*.dmg $(DIST_DIR)/releases/v$(VERSION)/macos/; \
		echo "DMG packages moved to: $(DIST_DIR)/releases/v$(VERSION)/macos/"; \
	fi
	@if ls $(DIST_DIR)/$(PROJECT_NAME)-$(VERSION)-*.pkg 1> /dev/null 2>&1; then \
		mv $(DIST_DIR)/$(PROJECT_NAME)-$(VERSION)-*.pkg $(DIST_DIR)/releases/v$(VERSION)/macos/; \
		echo "PKG packages moved to: $(DIST_DIR)/releases/v$(VERSION)/macos/"; \
	fi

# Package source release (organized)
package-source-release:
	@echo "Creating organized source release..."
	@mkdir -p $(DIST_DIR)/releases/v$(VERSION)/source
	@if [ -d .git ]; then \
		git archive --format=tar.gz --prefix=$(PROJECT_NAME)-$(VERSION)/ HEAD > $(DIST_DIR)/releases/v$(VERSION)/source/$(PROJECT_NAME)-$(VERSION)-source.tar.gz; \
		echo "Source archive created: $(DIST_DIR)/releases/v$(VERSION)/source/$(PROJECT_NAME)-$(VERSION)-source.tar.gz"; \
	else \
		echo "Not a git repository, skipping source archive"; \
	fi

# Create release notes
release-notes:
	@echo "Creating release notes for v$(VERSION)..."
	@mkdir -p $(DIST_DIR)/releases/v$(VERSION)
	@echo "# simple-smtp-mailer v$(VERSION) Release Notes" > $(DIST_DIR)/releases/v$(VERSION)/RELEASE_NOTES.md
	@echo "" >> $(DIST_DIR)/releases/v$(VERSION)/RELEASE_NOTES.md
	@echo "## Release Date" >> $(DIST_DIR)/releases/v$(VERSION)/RELEASE_NOTES.md
	@echo "$(shell date '+%Y-%m-%d')" >> $(DIST_DIR)/releases/v$(VERSION)/RELEASE_NOTES.md
	@echo "" >> $(DIST_DIR)/releases/v$(VERSION)/RELEASE_NOTES.md
	@echo "## What's New" >> $(DIST_DIR)/releases/v$(VERSION)/RELEASE_NOTES.md
	@echo "- Complete package system with FHS compliance" >> $(DIST_DIR)/releases/v$(VERSION)/RELEASE_NOTES.md
	@echo "- OAuth2 helper tools for multiple email providers" >> $(DIST_DIR)/releases/v$(VERSION)/RELEASE_NOTES.md
	@echo "- Professional installer packages for all platforms" >> $(DIST_DIR)/releases/v$(VERSION)/RELEASE_NOTES.md
	@echo "- Comprehensive documentation and examples" >> $(DIST_DIR)/releases/v$(VERSION)/RELEASE_NOTES.md
	@echo "" >> $(DIST_DIR)/releases/v$(VERSION)/RELEASE_NOTES.md
	@echo "## Package Types" >> $(DIST_DIR)/releases/v$(VERSION)/RELEASE_NOTES.md
	@echo "- **Linux**: DEB, RPM, TGZ packages" >> $(DIST_DIR)/releases/v$(VERSION)/RELEASE_NOTES.md
	@echo "- **macOS**: DMG and PKG installers" >> $(DIST_DIR)/releases/v$(VERSION)/RELEASE_NOTES.md
	@echo "- **Windows**: NSIS installer (MSI)" >> $(DIST_DIR)/releases/v$(VERSION)/RELEASE_NOTES.md
	@echo "- **Source**: Complete source code archive" >> $(DIST_DIR)/releases/v$(VERSION)/RELEASE_NOTES.md
	@echo "" >> $(DIST_DIR)/releases/v$(VERSION)/RELEASE_NOTES.md
	@echo "## Installation" >> $(DIST_DIR)/releases/v$(VERSION)/RELEASE_NOTES.md
	@echo "See the documentation in the docs/ directory for detailed installation instructions." >> $(DIST_DIR)/releases/v$(VERSION)/RELEASE_NOTES.md
	@echo "Release notes created: $(DIST_DIR)/releases/v$(VERSION)/RELEASE_NOTES.md"

# Create complete release package
release: package-all release-notes
	@echo "Complete release v$(VERSION) created in: $(DIST_DIR)/releases/v$(VERSION)/"
	@echo "Contents:"
	@find $(DIST_DIR)/releases/v$(VERSION)/ -type f -name "*.md" -o -name "*.deb" -o -name "*.rpm" -o -name "*.tar.gz" -o -name "*.dmg" -o -name "*.pkg" | sort
	@echo ""
	@echo "Release is ready for distribution!"

# Debian/Ubuntu package
package-debian: build
	@echo "Building DEB package with Debian script..."
	./scripts/build-debian.sh --package

# Red Hat package
package-redhat: build
	@echo "Building RPM package with Red Hat script..."
	./scripts/build-redhat.sh --package

# FreeBSD package
package-freebsd: build
	@echo "Building FreeBSD package with FreeBSD script..."
	./scripts/build-freebsd.sh --package

# Generic package (for unsupported distributions)
package-generic: build
	@echo "Building generic package for unsupported distribution..."
	@mkdir -p $(DIST_DIR)
	cd $(BUILD_DIR) && cpack -G TGZ
	mv $(BUILD_DIR)/$(PROJECT_NAME)-$(VERSION)-*.tar.gz $(DIST_DIR)/
	@echo "Generic package created: $(DIST_DIR)/$(PROJECT_NAME)-$(VERSION)-*.tar.gz"

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
else ifeq ($(PLATFORM),linux)
	@echo "Detecting Linux distribution for package building..."
	@if [ -f /etc/debian_version ] || ( [ -f /etc/os-release ] && grep -q "debian\|ubuntu\|mint\|pop\|elementary\|zorin\|deepin" /etc/os-release ); then \
		echo "Detected Debian-based distribution, building DEB package..."; \
		$(MAKE) package-deb; \
	elif [ -f /etc/redhat-release ] || ( [ -f /etc/os-release ] && grep -q "rhel\|centos\|fedora\|rocky\|alma\|amazon\|oracle\|scientific" /etc/os-release ); then \
		echo "Detected Red Hat-based distribution, building RPM package..."; \
		$(MAKE) package-rpm; \
	elif [ -f /etc/arch-release ] || ( [ -f /etc/os-release ] && grep -q "arch\|manjaro\|endeavour\|garuda" /etc/os-release ); then \
		echo "Detected Arch-based distribution, building generic package..."; \
		$(MAKE) package-generic; \
	elif [ -f /etc/alpine-release ] || ( [ -f /etc/os-release ] && grep -q "alpine" /etc/os-release ); then \
		echo "Detected Alpine Linux, building generic package..."; \
		$(MAKE) package-generic; \
	elif [ -f /etc/os-release ] && grep -q "opensuse\|suse\|leap\|tumbleweed" /etc/os-release; then \
		echo "Detected openSUSE/SUSE distribution, building RPM package..."; \
		$(MAKE) package-rpm; \
	elif [ -f /etc/os-release ] && grep -q "gentoo" /etc/os-release; then \
		echo "Detected Gentoo distribution, building generic package..."; \
		$(MAKE) package-generic; \
	else \
		echo "Unknown Linux distribution, attempting both RPM and DEB..."; \
		$(MAKE) package-rpm package-deb; \
	fi
else
	@echo "Platform $(PLATFORM) not supported for packaging"
endif

# Detect Linux distribution
detect-distro:
ifeq ($(PLATFORM),linux)
	@echo "Detecting Linux distribution..."
	@if [ -f /etc/debian_version ]; then \
		echo "Distribution: Debian/Ubuntu (detected via /etc/debian_version)"; \
	elif [ -f /etc/redhat-release ]; then \
		echo "Distribution: Red Hat-based (detected via /etc/redhat-release)"; \
		cat /etc/redhat-release; \
	elif [ -f /etc/arch-release ]; then \
		echo "Distribution: Arch Linux (detected via /etc/arch-release)"; \
	elif [ -f /etc/alpine-release ]; then \
		echo "Distribution: Alpine Linux (detected via /etc/alpine-release)"; \
	elif [ -f /etc/os-release ]; then \
		echo "Distribution: Detected via /etc/os-release"; \
		grep "^PRETTY_NAME=" /etc/os-release | cut -d'"' -f2; \
		grep "^ID=" /etc/os-release | cut -d'=' -f2; \
		grep "^VERSION_ID=" /etc/os-release | cut -d'=' -f2; \
	else \
		echo "Distribution: Unknown (no standard release files found)"; \
	fi
else
	@echo "Not a Linux platform"
endif

# Platform-specific build scripts
build-script:
ifeq ($(PLATFORM),macos)
	@echo "Using macOS build script..."
	./scripts/build-macos.sh
else ifeq ($(PLATFORM),linux)
	@echo "Detecting Linux distribution for appropriate build script..."
	@if [ -f /etc/debian_version ] || [ -f /etc/os-release ] && grep -q "debian\|ubuntu" /etc/os-release; then \
		echo "Using Debian/Ubuntu build script..."; \
		./scripts/build-debian.sh; \
	elif [ -f /etc/redhat-release ] || [ -f /etc/os-release ] && grep -q "rhel\|centos\|fedora" /etc/os-release; then \
		echo "Using Red Hat build script..."; \
		./scripts/build-redhat.sh; \
	else \
		echo "Unknown Linux distribution, using generic build..."; \
		$(MAKE) build; \
	fi
else
	@echo "Platform $(PLATFORM) not supported by build scripts"
endif

# Debian/Ubuntu specific build
build-debian:
	@echo "Building with Debian/Ubuntu script..."
	./scripts/build-debian.sh

# Red Hat/CentOS/Fedora specific build  
build-redhat:
	@echo "Building with Red Hat script..."
	./scripts/build-redhat.sh

# FreeBSD specific build
build-freebsd:
	@echo "Building with FreeBSD script..."
	./scripts/build-freebsd.sh

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
build-release: $(BUILD_DIR)
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

# Show project information
info:
	@echo "╔══════════════════════════════════════════════════════════════════════════════╗"
	@echo "║                              PROJECT INFO                                    ║"
	@echo "╚══════════════════════════════════════════════════════════════════════════════╝"
	@echo "  Project Name:    $(PROJECT_NAME)"
	@echo "  Version:         $(VERSION)"
	@echo "  Platform:        $(PLATFORM)"
	@echo "  Compiler:        $(CXX)"
	@echo "  Build Directory: $(BUILD_DIR)"
	@echo "  Source Directory: $(SRC_DIR)"
	@echo "  Include Directory: $(INCLUDE_DIR)"
	@echo "  Config Directory: $(CONFIG_DIR)"
	@echo ""

# Show build status
status:
	@echo "╔══════════════════════════════════════════════════════════════════════════════╗"
	@echo "║                              BUILD STATUS                                    ║"
	@echo "╚══════════════════════════════════════════════════════════════════════════════╝"
	@if [ -d "$(BUILD_DIR)" ]; then \
		echo "  Build Directory: ✅ Exists ($(BUILD_DIR))"; \
		if [ -f "$(BUILD_DIR)/bin/$(PROJECT_NAME)" ]; then \
			echo "  Executable:      ✅ Built ($(BUILD_DIR)/bin/$(PROJECT_NAME))"; \
			echo "  Architecture:    $(shell file $(BUILD_DIR)/bin/$(PROJECT_NAME) | grep -o 'Mach-O.*' || echo 'Unknown')"; \
		else \
			echo "  Executable:      ❌ Not built"; \
		fi; \
		if [ -f "$(BUILD_DIR)/lib/lib$(PROJECT_NAME).dylib" ] || [ -f "$(BUILD_DIR)/lib/lib$(PROJECT_NAME).so" ]; then \
			echo "  Library:         ✅ Built"; \
		else \
			echo "  Library:         ❌ Not built"; \
		fi; \
	else \
		echo "  Build Directory: ❌ Does not exist"; \
		echo "  Run 'make build' to create it"; \
	fi
	@echo ""

# Show dependencies status
deps-status:
	@echo "╔══════════════════════════════════════════════════════════════════════════════╗"
	@echo "║                            DEPENDENCIES STATUS                             ║"
	@echo "╚══════════════════════════════════════════════════════════════════════════════╝"
	@echo "  Platform:        $(PLATFORM)"
	@echo "  Compiler:        $(shell which $(CXX) 2>/dev/null || echo "❌ Not found")"
	@echo "  CMake:           $(shell which cmake 2>/dev/null || echo "❌ Not found")"
	@echo "  Make:            $(shell which make 2>/dev/null || echo "❌ Not found")"
	@echo "  pkg-config:      $(shell which pkg-config 2>/dev/null || echo "❌ Not found")"
	@echo "  OpenSSL:         $(shell pkg-config --exists openssl && echo "✅ Found" || echo "❌ Not found")"
ifeq ($(PLATFORM),macos)
	@echo "  Homebrew:        $(shell which brew 2>/dev/null || echo "❌ Not found")"
	@echo "  Xcode Tools:     $(shell xcode-select -p 2>/dev/null && echo "✅ Installed" || echo "❌ Not installed")"
else
	@echo "  Build Tools:     $(shell which g++ 2>/dev/null && echo "✅ Found" || echo "❌ Not found")"
endif
	@echo ""

# Quick build check
check:
	@echo "╔══════════════════════════════════════════════════════════════════════════════╗"
	@echo "║                              QUICK CHECK                                     ║"
	@echo "╚══════════════════════════════════════════════════════════════════════════════╝"
	@if [ -f "$(BUILD_DIR)/bin/$(PROJECT_NAME)" ]; then \
		echo "✅ Build successful - executable found"; \
		echo "  Location: $(BUILD_DIR)/bin/$(PROJECT_NAME)"; \
		echo "  Size: $(shell ls -lh $(BUILD_DIR)/bin/$(PROJECT_NAME) | awk '{print $$5}')"; \
		echo "  Architecture: $(shell file $(BUILD_DIR)/bin/$(PROJECT_NAME) | grep -o 'Mach-O.*' || echo 'Unknown')"; \
	else \
		echo "❌ Build not found or failed"; \
		echo "  Run 'make build' to build the project"; \
	fi
	@echo ""

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
	# For DEB building (Ubuntu/Debian)
	sudo apt-get install -y fakeroot devscripts build-essential
	# For RPM building (optional, not recommended on Ubuntu)
	# sudo apt-get install -y rpm rpm-build alien
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
	@echo "╔══════════════════════════════════════════════════════════════════════════════╗"
	@echo "║                           simple-smtp-mailer Makefile Help                         ║"
	@echo "╚══════════════════════════════════════════════════════════════════════════════╝"
	@echo ""
	@echo "📋 BUILD TARGETS:"
	@echo "  all              - Build the project (default target)"
	@echo "  build            - Build using CMake with default settings"
	@echo "  build-script     - Build using platform-specific build script ⭐ RECOMMENDED"
	@echo "                    macOS: 🚀 SMART auto-detects architecture, Linux: native build"
	@echo "  build-debian     - Build using Debian/Ubuntu script"
	@echo "  build-redhat     - Build using Red Hat script"
	@echo "  build-freebsd    - Build using FreeBSD script"
	@echo "  clean            - Clean build artifacts and directories"
	@echo "  install          - Install to system-wide location"
	@echo "  uninstall        - Remove from system"
	@echo ""
	@echo "🧪 TESTING:"
	@echo "  test             - Run all tests"
	@echo ""
	@echo "📦 PACKAGING:"
	@echo "  package          - 🚀 SMART: Auto-detect OS and build appropriate package"
	@echo "  package-script   - Build packages using platform-specific scripts"
	@echo "  package-debian   - Build DEB package using Debian script"
	@echo "  package-redhat   - Build RPM package using Red Hat script"
	@echo "  package-freebsd  - Build FreeBSD package using FreeBSD script"
	@echo ""
	@echo "🚀 RELEASE MANAGEMENT:"
	@echo "  package-all      - Build organized release packages for all platforms"
	@echo "  release          - Create complete release with notes and organization"
	@echo "  release-notes    - Generate release notes for current version"
ifeq ($(PLATFORM),macos)
	@echo "  package-dmg      - Build macOS DMG package for distribution"
else
	@echo "  package-rpm      - Build RPM package (Red Hat/CentOS/Fedora)"
	@echo "  package-deb      - Build DEB package (Debian/Ubuntu)"
	@echo "  package-generic  - Build generic TGZ package (unsupported distributions)"
endif
	@echo ""
	@echo "🏗️  ARCHITECTURE-SPECIFIC BUILDS:"
ifeq ($(PLATFORM),macos)
	@echo "  build-universal  - Build universal binary (Intel + Apple Silicon) ⭐ RECOMMENDED"
	@echo "  build-intel      - Build for Intel Macs only (x86_64)"
	@echo "  build-arm64      - Build for Apple Silicon Macs only (arm64)"
else
	@echo "  build-32         - Build 32-bit version for Linux"
	@echo "  build-64         - Build 64-bit version for Linux"
	@echo "  build-multiarch  - Build for both 32-bit and 64-bit architectures"
endif
	@echo ""
	@echo "🔧 DEVELOPMENT:"
	@echo "  debug            - Build with debug information and symbols"
	@echo "  release          - Build optimized release version"
	@echo "  analyze          - Run static analysis with clang-tidy"
	@echo "  format           - Format code using clang-format"
	@echo "  check-style      - Check code style without modifying files"
	@echo ""
	@echo "📚 DEPENDENCIES:"
	@echo "  deps             - Install required dependencies for current platform"
	@echo ""
	@echo "🔍 SYSTEM INFO & DIAGNOSTICS:"
	@echo "  detect-distro    - 🔍 Detect and display Linux distribution information"
	@echo "  status           - Show build status and system information"
	@echo "  info             - Show project information and configuration"
ifeq ($(PLATFORM),linux)
	@echo ""
	@echo "🐳 DOCKER:"
	@echo "  docker-build     - Build Docker image"
	@echo "  docker-run       - Run Docker container"
endif
	@echo ""
		@echo "❓ HELP & UTILITIES:"
	@echo "  help             - Show this detailed help message"
	@echo "  deps-status      - Show dependencies status and requirements"
	@echo "  check            - Quick build check and validation"
	@echo ""
	@echo "╔══════════════════════════════════════════════════════════════════════════════╗"
	@echo "║                              PLATFORM INFO                                   ║"
	@echo "╚══════════════════════════════════════════════════════════════════════════════╝"
	@echo "  Platform:        $(PLATFORM)"
	@echo "  Compiler:        $(CXX)"
	@echo "  Version:         $(VERSION)"
	@echo "  Parallel Jobs:   $(PARALLEL_JOBS)"
	@echo "  Build Directory: $(BUILD_DIR)"
	@echo "  Install Prefix:  $(INSTALL_PREFIX)"
	@echo ""
	@echo "╔══════════════════════════════════════════════════════════════════════════════╗"
	@echo "║                              QUICK START                                     ║"
	@echo "╚══════════════════════════════════════════════════════════════════════════════╝"
ifeq ($(PLATFORM),macos)
	@echo "  🍎 macOS Quick Start:"
	@echo "    1. make deps                    # Install dependencies via Homebrew"
	@echo "    2. make build-script            # 🚀 SMART: Auto-detects your Mac's architecture!"
	@echo "    3. make package-dmg             # Create distribution package"
	@echo "    4. sudo make install            # Install to system"
	@echo ""
	@echo "  🔍 macOS Architecture Detection:"
	@echo "    🚀 SMART: Automatically detects your Mac's architecture by default!"
	@echo "    • Intel Macs → x86_64 binary (smaller, faster)"
	@echo "    • Apple Silicon → arm64 binary (native performance)"
	@echo "    • Universal → Both architectures (larger, compatible with all Macs)"
	@echo "    • Override: use 'make build-intel' or 'make build-arm64' for specific builds"
else
	@echo "  🐧 Linux Quick Start:"
	@echo "    1. make deps                    # Install dependencies via apt/yum/dnf"
	@echo "    2. make build                   # Build for current architecture"
	@echo "    3. make package                 # 🚀 SMART: Auto-detect OS and create packages"
	@echo "    4. sudo make install            # Install to system"
	@echo ""
	@echo "  🔍 Linux Distribution Detection:"
	@echo "    • Debian/Ubuntu/Mint → DEB packages"
	@echo "    • RHEL/CentOS/Fedora → RPM packages"
	@echo "    • openSUSE/SUSE → RPM packages"
	@echo "    • Arch/Gentoo/Alpine → Generic TGZ packages"
endif
	@echo ""
	@echo "╔══════════════════════════════════════════════════════════════════════════════╗"
	@echo "║                              EXAMPLES                                        ║"
	@echo "╚══════════════════════════════════════════════════════════════════════════════╝"
	@echo "  # Clean build with debug info"
	@echo "  make clean && make debug"
	@echo ""
	@echo "  # Build and run tests"
	@echo "  make build && make test"
	@echo ""
	@echo "  # Build and create package (macOS)"
ifeq ($(PLATFORM),macos)
	@echo "  make build-script && make package-dmg          # 🚀 SMART: Auto-detect architecture"
	@echo "  make build-universal && make package-dmg       # Force universal binary"
else
	@echo "  make build-64 && make package"
endif
	@echo ""
	@echo "  # Install dependencies and build"
	@echo "  make deps && make build"
	@echo ""
	@echo "  # Smart packaging examples:"
	@echo "  make package                     # Auto-detect OS and build package"
	@echo "  make detect-distro               # Check detected distribution"
	@echo "  make package-deb                 # Force DEB package (Debian/Ubuntu)"
	@echo "  make package-rpm                 # Force RPM package (Red Hat/CentOS)"
	@echo ""
	@echo "╔══════════════════════════════════════════════════════════════════════════════╗"
	@echo "║                              TROUBLESHOOTING                                 ║"
	@echo "╚══════════════════════════════════════════════════════════════════════════════╝"
	@echo "  • If build fails, try: make clean && make build"
	@echo "  • If dependencies are missing: make deps"
	@echo "  • For debug info: make debug"
	@echo "  • Check platform detection: make help"
	@echo ""
	@echo "  📦 PACKAGING ISSUES:"
	@echo "    • Check detected distribution: make detect-distro"
	@echo "    • Force specific package type: make package-deb or make package-rpm"
	@echo "    • For unsupported distros: make package-generic"
	@echo "    • Verify build completion before packaging: make build"
	@echo ""
ifeq ($(PLATFORM),macos)
	@echo "  🍎 macOS ARCHITECTURE ISSUES:"
	@echo "    • Use 'make build-script' for auto-detection (recommended)"
	@echo "    • Force Intel-only: make build-intel"
	@echo "    • Force Apple Silicon: make build-arm64"
	@echo "    • Universal binary: make build-universal (requires both arch libs)"
	@echo "    • If ARM64 linking fails on Intel Mac: use 'make build-intel'"
endif
	@echo ""
	@echo "╔══════════════════════════════════════════════════════════════════════════════╗"
	@echo "║                              LINUX DISTRIBUTION SUPPORT                      ║"
	@echo "╚══════════════════════════════════════════════════════════════════════════════╝"
	@echo "  🐧 FULLY SUPPORTED (Native packages):"
	@echo "    • Debian/Ubuntu family: Ubuntu, Debian, Linux Mint, Pop!_OS, Elementary"
	@echo "    • Red Hat family: RHEL, CentOS, Fedora, Rocky Linux, AlmaLinux, Amazon Linux"
	@echo "    • SUSE family: openSUSE Leap, Tumbleweed, SUSE Linux Enterprise"
	@echo ""
	@echo "  🔧 PARTIALLY SUPPORTED (Generic packages):"
	@echo "    • Arch family: Arch Linux, Manjaro, EndeavourOS, Garuda"
	@echo "    • Alpine Linux, Gentoo, Void Linux"
	@echo "    • Other distributions: Generic TGZ packages"
	@echo ""
	@echo "╔══════════════════════════════════════════════════════════════════════════════╗"
	@echo "║                              MORE INFO                                       ║"
	@echo "╚══════════════════════════════════════════════════════════════════════════════╝"
	@echo "  • Project: $(PROJECT_NAME) v$(VERSION)"
	@echo "  • README:  README.md"
ifeq ($(PLATFORM),macos)
	@echo "  • macOS Guide: docs/macos-build.md"
	@echo "  • Build Script: scripts/build-macos.sh"
endif
	@echo "  • CMake Config: CMakeLists.txt"
	@echo ""
	@echo "💡 Tip: Use 'make help' anytime to see this information again!"

.PHONY: all clean install uninstall test package package-rpm package-deb package-dmg build-32 build-64 build-multiarch build-universal build-intel build-arm64 debug release analyze format check-style deps docker-build docker-run help info status deps-status check
