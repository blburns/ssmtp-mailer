# Centralized Release Scripts

This directory contains scripts for managing centralized releases of ssmtp-mailer packages from multiple build systems.

## Scripts Overview

### 1. `collect-packages.sh` - Package Collection Helper

**Purpose**: Collects packages from local build systems and prepares them for centralized release.

**Usage**:
```bash
# Collect packages from local build
./scripts/collect-packages.sh --collect

# Create collection summary
./scripts/collect-packages.sh --summary

# Do both (recommended)
./scripts/collect-packages.sh --all
```

**What it does**:
- Creates organized directory structure by platform
- Copies packages from local build to collection directory
- Generates platform-specific information files
- Creates comprehensive collection summary

**Output**: `dist/collections/v0.2.0/` directory with organized packages

### 2. `centralized-release.sh` - Centralized Release Manager

**Purpose**: Manages the final release process from a centralized location.

**Usage**:
```bash
# Scan for available packages
./scripts/centralized-release.sh --scan

# Collect packages from local release directory
./scripts/centralized-release.sh --collect

# Create GitHub release with existing packages
./scripts/centralized-release.sh --release

# Run complete process (recommended)
./scripts/centralized-release.sh --all
```

**What it does**:
- Scans for packages in centralized and local directories
- Creates comprehensive release notes
- Manages GitHub releases (create/update)
- Uploads all packages to GitHub release
- Publishes the release

## Workflow

### Step 1: Build and Collect on Each System

On each build system (macOS, Linux, Windows):

```bash
# 1. Build packages
make release  # or appropriate build command

# 2. Collect packages
./scripts/collect-packages.sh --all

# 3. Transfer collection directory to centralized system
scp -r dist/collections/v0.2.0/ user@central-system:/path/to/ssmtp-mailer/dist/
```

### Step 2: Centralized Release

On your centralized system:

```bash
# 1. Ensure all package collections are transferred
ls -la dist/collections/v0.2.0/

# 2. Create centralized release
./scripts/centralized-release.sh --all
```

## Directory Structure

```
ssmtp-mailer/
├── dist/                        # Standard distribution directory
│   ├── releases/                # Local build releases
│   │   └── v0.2.0/             # Version-specific releases
│   ├── collections/             # Package collections from build systems
│   │   └── v0.2.0/             # Version-specific collections
│   │       ├── COLLECTION_SUMMARY.md
│   │       ├── macos-x86_64/
│   │       ├── linux-debian/
│   │       └── windows/
│   └── centralized/             # Final centralized release packages
│       └── v0.2.0/             # Version-specific centralized release
└── scripts/
    ├── collect-packages.sh      # Package collection
    └── centralized-release.sh   # Centralized release
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

## Example Workflow

### On macOS Build System
```bash
make release
./scripts/collect-packages.sh --all
scp -r dist/collections/v0.2.0/ user@central:/path/to/ssmtp-mailer/dist/
```

### On Linux Build System
```bash
./scripts/build-debian.sh
./scripts/collect-packages.sh --all
scp -r dist/collections/v0.2.0/ user@central:/path/to/ssmtp-mailer/dist/
```

### On Centralized System
```bash
# Verify all packages are transferred
ls -la package-collection/v0.2.0/*/

# Create release
./scripts/centralized-release.sh --all
```

## Benefits

1. **Centralized Management**: All releases managed from one location
2. **Cross-Platform Support**: Easy to include packages from multiple systems
3. **Consistent Process**: Same release process regardless of build platform
4. **Automated Workflow**: Reduces manual errors and saves time
5. **Comprehensive Documentation**: Automatic generation of release notes
6. **Version Control**: Easy to track what packages are included in each release

## Troubleshooting

### Common Issues

1. **No packages found**: Ensure packages were built and collection script was run
2. **GitHub CLI errors**: Check authentication with `gh auth status`
3. **Permission errors**: Verify file permissions and GitHub repository access
4. **Duplicate packages**: This is normal - script scans both local and centralized directories

### Getting Help

1. Check script help: `./scripts/script-name.sh --help`
2. Review generated log files and summaries
3. Verify all prerequisites are met
4. Check GitHub CLI status and permissions

## Support

For detailed documentation, see: `docs/CENTRALIZED_RELEASE_WORKFLOW.md`
