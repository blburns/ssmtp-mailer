#!/usr/bin/env python3
"""
OAuth2 Helper Launcher for ssmtp-mailer
This script provides a unified interface for OAuth2 authentication helpers.
"""

import sys
import os
import argparse
from pathlib import Path

def main():
    """Main function to launch OAuth2 helpers"""
    parser = argparse.ArgumentParser(
        description="OAuth2 Helper Launcher for ssmtp-mailer",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s gmail                    # Launch Gmail OAuth2 helper
  %(prog)s office365                # Launch Office 365 OAuth2 helper
  %(prog)s --list                   # List available OAuth2 helpers
  %(prog)s --help                   # Show this help message
        """
    )
    
    parser.add_argument(
        'provider',
        nargs='?',
        choices=['gmail', 'office365', 'sendgrid', 'amazon-ses', 'protonmail', 'zoho-mail', 'mailgun', 'fastmail'],
        help='OAuth2 provider to use'
    )
    
    parser.add_argument(
        '--list', '-l',
        action='store_true',
        help='List available OAuth2 providers'
    )
    
    parser.add_argument(
        '--version', '-v',
        action='version',
        version='ssmtp-mailer OAuth2 Helper v1.0.0'
    )
    
    args = parser.parse_args()
    
    if args.list:
        show_available_providers()
        return
    
    if not args.provider:
        parser.print_help()
        print("\n❌ Please specify an OAuth2 provider.")
        print("   Use --list to see available providers.")
        return
    
    launch_oauth2_helper(args.provider)

def show_available_providers():
    """Show available OAuth2 providers"""
    print("🔐 Available OAuth2 Providers:")
    print("=" * 40)
    print()
    
    providers = {
        'gmail': {
            'name': 'Gmail / Google Workspace',
            'description': 'OAuth2 authentication for Gmail and Google Workspace',
            'script': 'python/gmail-oauth2-helper.py',
            'requirements': ['requests'],
            'features': [
                'Google Service Account support',
                'Gmail API integration',
                'Automatic token refresh'
            ]
        },
        'office365': {
            'name': 'Office 365 / Microsoft Graph',
            'description': 'OAuth2 authentication for Office 365 and Microsoft Graph',
            'script': 'python/office365-oauth2-helper.py',
            'requirements': ['requests'],
            'features': [
                'Microsoft Graph API support',
                'Office 365 integration',
                'Tenant-based authentication'
            ]
        },
        'sendgrid': {
            'name': 'SendGrid',
            'description': 'OAuth2 authentication for SendGrid API',
            'script': 'python/sendgrid-oauth2-helper.py',
            'requirements': ['requests'],
            'features': [
                'SendGrid API integration',
                'Transactional email support',
                'Webhook management'
            ]
        },
        'amazon-ses': {
            'name': 'Amazon SES',
            'description': 'OAuth2 authentication for Amazon Simple Email Service',
            'script': 'python/amazon-ses-oauth2-helper.py',
            'requirements': ['requests'],
            'features': [
                'AWS SES integration',
                'Regional endpoint support',
                'High deliverability'
            ]
        },
        'protonmail': {
            'name': 'ProtonMail',
            'description': 'OAuth2 authentication for ProtonMail (privacy-focused email)',
            'script': 'python/protonmail-oauth2-helper.py',
            'requirements': ['requests'],
            'features': [
                'ProtonMail integration',
                'Privacy-focused email',
                'End-to-end encryption'
            ]
        },
        'zoho-mail': {
            'name': 'Zoho Mail',
            'description': 'OAuth2 authentication for Zoho Mail (business email)',
            'script': 'python/zoho-mail-oauth2-helper.py',
            'requirements': ['requests'],
            'features': [
                'Zoho Mail integration',
                'Business email support',
                'API-based access'
            ]
        },
        'mailgun': {
            'name': 'Mailgun',
            'description': 'OAuth2 authentication for Mailgun (email API service)',
            'script': 'python/mailgun-oauth2-helper.py',
            'requirements': ['requests'],
            'features': [
                'Mailgun API integration',
                'Transactional email',
                'Email analytics'
            ]
        },
        'fastmail': {
            'name': 'Fastmail',
            'description': 'OAuth2 authentication for Fastmail (professional email hosting)',
            'script': 'python/fastmail-oauth2-helper.py',
            'requirements': ['requests'],
            'features': [
                'Fastmail integration',
                'Professional hosting',
                'IMAP/SMTP support'
            ]
        }
    }
    
    for key, provider in providers.items():
        print(f"📧 {provider['name']}")
        print(f"   Key: {key}")
        print(f"   Description: {provider['description']}")
        print(f"   Script: {provider['script']}")
        print(f"   Requirements: {', '.join(provider['requirements'])}")
        print(f"   Features:")
        for feature in provider['features']:
            print(f"     • {feature}")
        print()

def launch_oauth2_helper(provider):
    """Launch the specified OAuth2 helper"""
    script_dir = Path(__file__).parent
    
    if provider == 'gmail':
        script_path = script_dir / 'python' / 'gmail-oauth2-helper.py'
    elif provider == 'office365':
        script_path = script_dir / 'python' / 'office365-oauth2-helper.py'
    elif provider == 'sendgrid':
        script_path = script_dir / 'python' / 'sendgrid-oauth2-helper.py'
    elif provider == 'amazon-ses':
        script_path = script_dir / 'python' / 'amazon-ses-oauth2-helper.py'
    elif provider == 'protonmail':
        script_path = script_dir / 'python' / 'protonmail-oauth2-helper.py'
    elif provider == 'zoho-mail':
        script_path = script_dir / 'python' / 'zoho-mail-oauth2-helper.py'
    elif provider == 'mailgun':
        script_path = script_dir / 'python' / 'mailgun-oauth2-helper.py'
    elif provider == 'fastmail':
        script_path = script_dir / 'python' / 'fastmail-oauth2-helper.py'
    else:
        print(f"❌ Unknown provider: {provider}")
        return
    
    if not script_path.exists():
        print(f"❌ OAuth2 helper script not found: {script_path}")
        return
    
    print(f"🚀 Launching {provider} OAuth2 helper...")
    print(f"📁 Script: {script_path}")
    print()
    
    # Check if required dependencies are available
    try:
        import requests
    except ImportError:
        print("❌ Missing required dependency: requests")
        print("   Please install it with: pip install requests")
        return
    
    # Launch the helper script
    try:
        # Change to the script directory and execute
        os.chdir(script_dir)
        os.execv(sys.executable, [sys.executable, str(script_path)])
    except Exception as e:
        print(f"❌ Failed to launch OAuth2 helper: {e}")
        print(f"   You can run it manually: python {script_path}")

if __name__ == "__main__":
    main()
