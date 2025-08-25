# Quick Setup: CentOS Custom Repository for EPEL 9 Packages

This guide provides a quick way to set up custom YUM repositories on CentOS to access EPEL 9 packages needed for building ssmtp-mailer.

## 🚀 **Quick Setup (Automated)**

### **Option 1: Use the Setup Script**
```bash
# From the project root directory
sudo ./scripts/setup-centos-fedora-repo.sh --all
```

This will:
- Install the custom repository file
- Import GPG keys
- Test repository access
- Enable the repository

### **Option 2: Manual Setup**
```bash
# 1. Copy repository file
sudo cp etc/yum.repos.d/custom.repo /etc/yum.repos.d/

# 2. Import GPG keys
sudo rpm --import https://dl.fedoraproject.org/pub/epel/RPM-GPG-KEY-EPEL-9
sudo rpm --import https://dl.fedoraproject.org/pub/epel/RPM-GPG-KEY-EPEL-8

# 3. Clear YUM cache
sudo yum clean all
sudo yum makecache

# 4. Enable repository
sudo yum-config-manager --enable epel9
```

## 📁 **Files Created**

- **`etc/yum.repos.d/custom.repo`** - Repository configuration file
- **`scripts/setup-centos-fedora-repo.sh`** - Automated setup script
- **`docs/installation/centos-fedora-repo-setup.md`** - Detailed documentation

## ⚡ **Quick Test**

After setup, test the repository:
```bash
# Search for packages
sudo yum --enablerepo=epel9 search cmake

# Install a package
sudo yum install --enablerepo=epel9 package-name
```

## 🧹 **Cleanup After Building**

```bash
# Disable repository
sudo yum-config-manager --disable epel9

# Remove repository file (optional)
sudo rm /etc/yum.repos.d/custom.repo

# Clear cache
sudo yum clean all
```

## ⚠️ **Important Notes**

- **Use with caution** - Mixing CentOS and EPEL packages can cause instability
- **Test thoroughly** - Always test in a development environment first
- **Temporary use** - These repositories are intended for build-time use only
- **Priority set** - CentOS packages are preferred over EPEL packages

## 🔗 **More Information**

For detailed setup instructions and troubleshooting, see:
- **`docs/installation/centos-fedora-repo-setup.md`** - Comprehensive guide
- **`scripts/setup-centos-fedora-repo.sh --help`** - Script help

## 📞 **Support**

If you encounter issues:
1. Check YUM logs: `/var/log/yum.log`
2. Verify repository connectivity
3. Test with minimal package installation first
4. Consider using only the EPEL repository you need
