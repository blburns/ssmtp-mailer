#!/bin/bash

echo "🔧 SMTP Setup for ssmtp-mailer"
echo "==============================="
echo ""

# Create config directory
mkdir -p ~/.config/ssmtp-mailer/domains

echo "This script will help you set up SMTP for email delivery."
echo ""

# Function to test SMTP configuration
test_smtp() {
    local server=$1
    local port=$2
    local username=$3
    local password=$4
    local from_email=$5
    local to_email=$6
    
    echo "Testing SMTP configuration..."
    echo "Server: $server:$port"
    echo "Username: $username"
    echo "From: $from_email"
    echo "To: $to_email"
    echo ""
    
    # Create test email file
    cat > /tmp/test_email.txt << EOF
From: $from_email
To: $to_email
Subject: Test Email from ssmtp-mailer Setup
Date: $(date -R)
MIME-Version: 1.0
Content-Type: text/plain; charset=UTF-8

This is a test email from the ssmtp-mailer setup script.

If you receive this email, your SMTP configuration is working correctly!

Best regards,
ssmtp-mailer
EOF

    # Test with curl
    if [ -n "$password" ]; then
        curl -s --url smtp://$server:$port \
             --user "$username:$password" \
             --ssl-reqd \
             --mail-from "$from_email" \
             --mail-rcpt "$to_email" \
             --upload-file /tmp/test_email.txt
    else
        curl -s --url smtp://$server:$port \
             --mail-from "$from_email" \
             --mail-rcpt "$to_email" \
             --upload-file /tmp/test_email.txt
    fi
    
    local result=$?
    rm -f /tmp/test_email.txt
    
    if [ $result -eq 0 ]; then
        echo "✅ SUCCESS! Email sent successfully."
        echo "Check your inbox for the test email."
        return 0
    else
        echo "❌ FAILED with exit code: $result"
        return 1
    fi
}

# Interactive setup
echo "Let's set up your SMTP configuration:"
echo ""

# Get email provider
echo "Choose your email provider:"
echo "1) Gmail"
echo "2) Outlook/Hotmail"
echo "3) Yahoo"
echo "4) Custom SMTP server"
echo "5) Skip setup (use system sendmail)"
echo ""
read -p "Enter your choice (1-5): " choice

case $choice in
    1)
        echo ""
        echo "Gmail Setup:"
        echo "1. Go to https://myaccount.google.com/security"
        echo "2. Enable 2-Factor Authentication"
        echo "3. Generate an App Password for 'Mail'"
        echo "4. Use your Gmail address and the App Password below"
        echo ""
        read -p "Enter your Gmail address: " gmail_email
        read -p "Enter your Gmail App Password: " gmail_password
        read -p "Enter recipient email for test: " test_email
        
        if test_smtp "smtp.gmail.com" "587" "$gmail_email" "$gmail_password" "$gmail_email" "$test_email"; then
            # Create domain config
            cat > ~/.config/ssmtp-mailer/domains/gmail.com.conf << EOF
[domain]
name = gmail.com
enabled = true
smtp_server = smtp.gmail.com
smtp_port = 587
auth_method = LOGIN
username = $gmail_email
password = $gmail_password
use_ssl = false
use_starttls = true
EOF
            echo "✅ Gmail configuration saved!"
        fi
        ;;
    2)
        echo ""
        echo "Outlook Setup:"
        echo "1. Go to https://account.microsoft.com/security"
        echo "2. Enable 2-Factor Authentication"
        echo "3. Generate an App Password"
        echo "4. Use your Outlook address and the App Password below"
        echo ""
        read -p "Enter your Outlook email: " outlook_email
        read -p "Enter your Outlook App Password: " outlook_password
        read -p "Enter recipient email for test: " test_email
        
        if test_smtp "smtp-mail.outlook.com" "587" "$outlook_email" "$outlook_password" "$outlook_email" "$test_email"; then
            # Create domain config
            cat > ~/.config/ssmtp-mailer/domains/outlook.com.conf << EOF
[domain]
name = outlook.com
enabled = true
smtp_server = smtp-mail.outlook.com
smtp_port = 587
auth_method = LOGIN
username = $outlook_email
password = $outlook_password
use_ssl = false
use_starttls = true
EOF
            echo "✅ Outlook configuration saved!"
        fi
        ;;
    3)
        echo ""
        echo "Yahoo Setup:"
        echo "1. Go to https://login.yahoo.com/account/security"
        echo "2. Enable 2-Factor Authentication"
        echo "3. Generate an App Password"
        echo "4. Use your Yahoo address and the App Password below"
        echo ""
        read -p "Enter your Yahoo email: " yahoo_email
        read -p "Enter your Yahoo App Password: " yahoo_password
        read -p "Enter recipient email for test: " test_email
        
        if test_smtp "smtp.mail.yahoo.com" "587" "$yahoo_email" "$yahoo_password" "$yahoo_email" "$test_email"; then
            # Create domain config
            cat > ~/.config/ssmtp-mailer/domains/yahoo.com.conf << EOF
[domain]
name = yahoo.com
enabled = true
smtp_server = smtp.mail.yahoo.com
smtp_port = 587
auth_method = LOGIN
username = $yahoo_email
password = $yahoo_password
use_ssl = false
use_starttls = true
EOF
            echo "✅ Yahoo configuration saved!"
        fi
        ;;
    4)
        echo ""
        echo "Custom SMTP Setup:"
        read -p "Enter SMTP server: " custom_server
        read -p "Enter SMTP port (default 587): " custom_port
        custom_port=${custom_port:-587}
        read -p "Enter username (or leave empty for no auth): " custom_username
        read -p "Enter password (or leave empty for no auth): " custom_password
        read -p "Enter from email: " custom_from
        read -p "Enter recipient email for test: " test_email
        
        if test_smtp "$custom_server" "$custom_port" "$custom_username" "$custom_password" "$custom_from" "$test_email"; then
            # Create domain config
            local domain=$(echo "$custom_from" | cut -d'@' -f2)
            cat > ~/.config/ssmtp-mailer/domains/$domain.conf << EOF
[domain]
name = $domain
enabled = true
smtp_server = $custom_server
smtp_port = $custom_port
auth_method = ${custom_username:+LOGIN}
username = $custom_username
password = $custom_password
use_ssl = false
use_starttls = true
EOF
            echo "✅ Custom SMTP configuration saved!"
        fi
        ;;
    5)
        echo ""
        echo "Skipping SMTP setup. Using system sendmail (may not work for external emails)."
        ;;
    *)
        echo "Invalid choice. Exiting."
        exit 1
        ;;
esac

echo ""
echo "Setup complete! Your SMTP configuration is ready."
echo ""
echo "To test email sending, you can now run:"
echo "  ./ssmtp-mailer send --from your-email@domain.com --to recipient@example.com --subject 'Test' --body 'Hello World'"
echo ""
