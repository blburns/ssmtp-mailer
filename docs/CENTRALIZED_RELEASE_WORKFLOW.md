# Centralized Release Workflow

This document explains how to use the centralized release workflow for ssmtp-mailer, which allows you to collect packages from multiple build systems and release them all at once.

## Overview

The centralized release workflow consists of two main components:

1. **Package Collection** (`scripts/collect-packages.sh`) - Run on each build system to collect packages
2. **Centralized Release** (`scripts/centralized-release.sh`) - Run on your main system to create the GitHub release

## Workflow Steps

### Step 1: Build Packages on Different Systems

Build packages on each target platform:

```bash
# On macOS
make release

# On Linux (Debian/Ubuntu)
./scripts/build-debian.sh

# On Linux (Red Hat/CentOS)
./scripts/build-redhat.sh

# On Windows
./scripts/build-windows.ps1
```

### Step 2: Collect Packages on Each System

After building, run the collection script on each system:

```bash
# Collect packages and create summary
./scripts/collect-packages.sh --all
```

This will:
- Create a `dist/collections/v0.2.0/` directory
- Organize packages by platform (e.g., `macos-x86_64/`, `linux-debian/`)
- Copy all packages from the local build
- Create platform-specific information files
- Generate a collection summary

### Step 3: Transfer Packages to Centralized System

Transfer the entire `dist/collections/v0.2.0/` directory to your centralized release system. You can use:

- **SCP/SFTP**: `scp -r dist/collections/v0.2.0/ user@central-system:/path/to/ssmtp-mailer/dist/`
- **Git**: Commit and push the collection directory
- **Cloud Storage**: Upload to Google Drive, Dropbox, etc.
- **Direct Copy**: If building on the same machine

### Step 4: Create Centralized Release

On your centralized system, run:

```bash
# Scan for available packages
./scripts/centralized-release.sh --scan

# Create and publish GitHub release
./scripts/centralized-release.sh --all
```

This will:
- Scan for all available packages
- Create comprehensive release notes
- Create a GitHub release (initially as draft)
- Upload all packages to the release
- Publish the release

## Directory Structure

```
ssmtp-mailer/
├── dist/                        # Standard distribution directory
│   ├── releases/                # Local build releases
│   │   └── v0.2.0/             # Version-specific releases
│   │       ├── macos/           # macOS packages
│   │       ├── linux/           # Linux packages
│   │       ├── windows/         # Windows packages
│   │       └── source/          # Source packages
│   ├── collections/             # Package collections from build systems
│   │   └── v0.2.0/             # Version-specific collections
│   │       ├── COLLECTION_SUMMARY.md
│   │       ├── macos-x86_64/    # macOS Intel packages
│   │       ├── linux-debian/    # Debian/Ubuntu packages
│   │       └── windows/         # Windows packages
│   └── centralized/             # Final centralized release packages
│       └── v0.2.0/             # Version-specific centralized release
└── scripts/
    ├── collect-packages.sh      # Package collection script
    └── centralized-release.sh   # Centralized release script
```

## Script Usage

### Package Collection Script

```bash
# Show help
./scripts/collect-packages.sh --help

# Collect packages from local build
./scripts/collect-packages.sh --collect

# Create collection summary
./scripts/collect-packages.sh --summary

# Do both (recommended)
./scripts/collect-packages.sh --all
```

### Centralized Release Script

```bash
# Show help
./scripts/centralized-release.sh --help

# Scan for available packages
./scripts/centralized-release.sh --scan

# Collect packages from local release directory
./scripts/centralized-release.sh --collect

# Create GitHub release with existing packages
./scripts/centralized-release.sh --release

# Run complete process (recommended)
./scripts/centralized-release.sh --all
```

## Example Workflow

### On macOS Build System

```bash
# 1. Build packages
make release

# 2. Collect packages
./scripts/collect-packages.sh --all

# 3. Transfer to centralized system
scp -r dist/collections/v0.2.0/ user@central-system:/path/to/ssmtp-mailer/dist/
```

### On Linux Build System

```bash
# 1. Build packages
./scripts/build-debian.sh

# 2. Collect packages
./scripts/collect-packages.sh --all

# 3. Transfer to centralized system
scp -r dist/collections/v0.2.0/ user@central-system:/path/to/ssmtp-mailer/dist/
```

### On Centralized System

```bash
# 1. Ensure all packages are transferred
ls -la dist/collections/v0.2.0/

# 2. Create centralized release
./scripts/centralized-release.sh --all
```

## Prerequisites

### On Build Systems

- ssmtp-mailer source code
- Build dependencies installed
- `scripts/collect-packages.sh` script

### On Centralized System

- ssmtp-mailer source code
- GitHub CLI (`gh`) installed and authenticated
- All package collections transferred
- `scripts/centralized-release.sh` script

## GitHub CLI Setup

Install and authenticate GitHub CLI:

```bash
# Install (macOS)
brew install gh

# Install (Ubuntu)
sudo apt install gh

# Install (Windows)
winget install GitHub.cli

# Authenticate
gh auth login
```

## Troubleshooting

### No Packages Found

- Ensure packages were built successfully
- Check that `dist/releases/v0.2.0/` exists
- Verify package file extensions are supported

### GitHub Release Issues

- Check GitHub CLI authentication: `gh auth status`
- Ensure you have write access to the repository
- Check if release tag already exists

### Package Transfer Issues

- Verify network connectivity
- Check file permissions
- Ensure sufficient disk space
- Use `rsync` for large transfers: `rsync -avz dist/collections/ user@system:/path/to/ssmtp-mailer/dist/`

## Benefits

1. **Centralized Management**: All releases managed from one location
2. **Cross-Platform Support**: Easy to include packages from multiple systems
3. **Consistent Process**: Same release process regardless of build platform
4. **Automated Workflow**: Reduces manual errors and saves time
5. **Comprehensive Documentation**: Automatic generation of release notes
6. **Version Control**: Easy to track what packages are included in each release

## Best Practices

1. **Always run collection script** after building packages
2. **Verify package integrity** before transferring (checksums are included)
3. **Test the centralized release script** with `--scan` first
4. **Keep collection directories** for audit purposes
5. **Use consistent naming** for package files across platforms
6. **Document any custom build processes** in platform info files

## Support

If you encounter issues:

1. Check the script help: `./scripts/script-name.sh --help`
2. Review the generated log files and summaries
3. Verify all prerequisites are met
4. Check GitHub CLI status and permissions
5. Review the package collection structure
