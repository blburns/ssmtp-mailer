# CentOS Custom Repository Setup for EPEL 9 Packages

This guide explains how to set up custom YUM repositories on CentOS to access EPEL 9 packages needed for building ssmtp-mailer.

## ⚠️ **Important Notes**

- **Use with caution**: Mixing CentOS and EPEL packages can cause system instability
- **Test thoroughly**: Always test in a development environment first
- **Backup**: Consider backing up your system before making changes
- **Temporary use**: These repositories are intended for build-time use only

## 📁 **Repository Configuration**

### **File Location**
Place the custom repository file in: `/etc/yum.repos.d/custom.repo`

### **File Contents**
```ini
# Custom YUM Repository Configuration for CentOS
# This file enables access to Fedora packages for building ssmtp-mailer
# Place this file in /etc/yum.repos.d/ and enable with: sudo yum-config-manager --enable epel9

[epel9]
name=Extra Packages for Enterprise Linux 9 - $basearch
baseurl=https://dl.fedoraproject.org/pub/epel/9/Everything/$basearch
enabled=0
gpgcheck=1
gpgkey=https://dl.fedoraproject.org/pub/epel/RPM-GPG-KEY-EPEL-9
priority=50
metadata_expire=86400
repo_gpgcheck=0
type=rpm-md
skip_if_unavailable=1

# Alternative: EPEL 8 repository (for older CentOS systems)
[epel8]
name=Extra Packages for Enterprise Linux 8 - $basearch
baseurl=https://dl.fedoraproject.org/pub/epel/8/Everything/$basearch
enabled=0
gpgcheck=1
gpgkey=https://dl.fedoraproject.org/pub/epel/RPM-GPG-KEY-EPEL-8
priority=50
metadata_expire=86400
repo_gpgcheck=0
type=rpm-md
skip_if_unavailable=1
```

## 🚀 **Setup Instructions**

### **Step 1: Copy Repository File**
```bash
# Copy the custom repository file to the system
sudo cp custom.repo /etc/yum.repos.d/

# Set proper permissions
sudo chmod 644 /etc/yum.repos.d/custom.repo
```

### **Step 2: Import GPG Keys**
```bash
# Import EPEL 9 GPG key
sudo rpm --import https://dl.fedoraproject.org/pub/epel/RPM-GPG-KEY-EPEL-9

# Import EPEL 8 GPG key (for older systems)
sudo rpm --import https://dl.fedoraproject.org/pub/epel/RPM-GPG-KEY-EPEL-8
```

### **Step 3: Clear YUM Cache**
```bash
# Clear existing YUM cache
sudo yum clean all

# Update YUM cache with new repositories
sudo yum makecache
```

### **Step 4: Enable Repository (Temporary)**
```bash
# Enable the EPEL 9 repository temporarily
sudo yum-config-manager --enable epel9

# Or enable EPEL 8 if you prefer (for older systems)
sudo yum-config-manager --enable epel8

# Verify repositories are enabled
sudo yum repolist enabled
```

## 🔧 **Usage Examples**

### **Install Specific Packages from Fedora**
```bash
# Install a package from the EPEL 9 repository
sudo yum install --enablerepo=epel9 package-name

# Install with specific repository priority
sudo yum install --enablerepo=epel9 --setopt=installonly_limit=0 package-name
```

### **Search for Packages**
```bash
# Search for packages in all repositories
sudo yum search package-name

# Search in specific repository
sudo yum --enablerepo=epel9 search package-name
```

### **List Available Packages**
```bash
# List all packages in EPEL 9 repository
sudo yum --enablerepo=epel9 list available | grep package-name
```

## 🛡️ **Safety Measures**

### **Repository Priority**
The repositories are configured with `priority=50` to ensure CentOS packages are preferred over EPEL packages.

### **GPG Checking**
GPG checking is enabled to verify package authenticity.

### **Skip if Unavailable**
The `skip_if_unavailable=1` option prevents YUM from failing if the repository is temporarily unavailable.

## 🔍 **Troubleshooting**

### **GPG Key Issues**
```bash
# If you encounter GPG key errors, you can temporarily disable checking
sudo yum install --enablerepo=epel9 --nogpgcheck package-name

# Or update the GPG key
sudo rpm --import https://dl.fedoraproject.org/pub/epel/RPM-GPG-KEY-EPEL-9
```

### **Repository Conflicts**
```bash
# Check for repository conflicts
sudo yum check-update

# Resolve conflicts manually
sudo yum update --enablerepo=epel9
```

### **Package Dependencies**
```bash
# Check package dependencies
sudo yum deplist package-name

# Install with dependency resolution
sudo yum install --enablerepo=epel9 --resolve package-name
```

## 🧹 **Cleanup**

### **Disable Repository After Use**
```bash
# Disable the EPEL 9 repository
sudo yum-config-manager --disable epel9

# Or remove the repository file entirely
sudo rm /etc/yum.repos.d/custom.repo
```

### **Restore System State**
```bash
# Clear YUM cache
sudo yum clean all

# Update system with only CentOS repositories
sudo yum update --disablerepo=epel9,epel8
```

## 📋 **Recommended Workflow**

1. **Setup**: Copy repository file and import GPG keys
2. **Build**: Enable repository and install required packages
3. **Test**: Verify the build works correctly
4. **Cleanup**: Disable repository and clean up
5. **Verify**: Ensure system stability after cleanup

## 🔗 **Alternative Approaches**

### **EPEL 9 Repository Only**
If you prefer a more conservative approach, use only the EPEL 9 repository:
```bash
sudo yum-config-manager --enable epel9
```

### **Container-Based Building**
Consider using containers for building to avoid system-level package conflicts:
```bash
# Use Docker with EPEL-compatible base
docker run -it --rm -v $(pwd):/workspace centos:8 bash
```

## 📞 **Support**

If you encounter issues:
1. Check the YUM error logs: `/var/log/yum.log`
2. Verify repository connectivity: `curl -I [repository-url]`
3. Test with minimal package installation first
4. Consider using only the EPEL repository you need

## ⚡ **Quick Test**

After setup, test with a simple package:
```bash
# Test repository access
sudo yum --enablerepo=epel9 search cmake

# If successful, proceed with your build
./scripts/build-redhat.sh
```
