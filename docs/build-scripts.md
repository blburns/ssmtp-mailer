# Build Scripts Guide

This guide covers the specialized build scripts available for different platforms in the ssmtp-mailer project.

## Makefile Integration

The Makefile now includes integrated support for all build scripts:

### Automatic Platform Detection

```bash
# Auto-detect platform and use appropriate script
make build-script

# Auto-detect platform and build appropriate package
make package-script
```
