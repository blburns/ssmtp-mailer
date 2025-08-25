#!/bin/bash

# Service Account Setup Script for ssmtp-mailer
# This script helps you set up Google Service Account authentication

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
CONFIG_DIR="/etc/ssmtp-mailer"
SERVICE_ACCOUNTS_DIR="$CONFIG_DIR/service-accounts"

# Functions
print_header() {
    echo -e "${BLUE}================================${NC}"
    echo -e "${BLUE}  Service Account Setup Script  ${NC}"
    echo -e "${BLUE}================================${NC}"
    echo
}

print_step() {
    echo -e "${YELLOW}[STEP $1]${NC} $2"
    echo
}

print_success() {
    echo -e "${GREEN}✅ $1${NC}"
}

print_error() {
    echo -e "${RED}❌ $1${NC}"
}

print_info() {
    echo -e "${BLUE}ℹ️  $1${NC}"
}

check_dependencies() {
    print_step "1" "Checking dependencies..."
    
    local missing_deps=()
    
    # Check for required commands
    command -v curl >/dev/null 2>&1 || missing_deps+=("curl")
    command -v jq >/dev/null 2>&1 || missing_deps+=("jq")
    command -v openssl >/dev/null 2>&1 || missing_deps+=("openssl")
    
    if [ ${#missing_deps[@]} -ne 0 ]; then
        print_error "Missing required dependencies: ${missing_deps[*]}"
        echo "Please install them first:"
        echo "  Ubuntu/Debian: sudo apt-get install curl jq openssl"
        echo "  CentOS/RHEL: sudo yum install curl jq openssl"
        echo "  macOS: brew install curl jq openssl"
        exit 1
    fi
    
    print_success "All dependencies are available"
}

create_directories() {
    print_step "2" "Creating configuration directories..."
    
    sudo mkdir -p "$CONFIG_DIR"
    sudo mkdir -p "$SERVICE_ACCOUNTS_DIR"
    sudo mkdir -p "$CONFIG_DIR/domains"
    sudo mkdir -p "$CONFIG_DIR/users"
    sudo mkdir -p "$CONFIG_DIR/mappings"
    
    # Set proper permissions
    sudo chown -R "$(whoami):$(whoami)" "$CONFIG_DIR"
    sudo chmod 755 "$CONFIG_DIR"
    sudo chmod 700 "$SERVICE_ACCOUNTS_DIR"
    
    print_success "Configuration directories created"
}

get_domain_info() {
    print_step "3" "Getting domain information..."
    
    echo "Please provide the following information:"
    echo
    
    read -p "Enter your domain (e.g., domain1.com): " DOMAIN
    read -p "Enter the email address to impersonate (e.g., mail-relay@$DOMAIN): " USER_EMAIL
    read -p "Enter your Google Cloud Project ID: " PROJECT_ID
    
    # Validate inputs
    if [[ -z "$DOMAIN" || -z "$USER_EMAIL" || -z "$PROJECT_ID" ]]; then
        print_error "All fields are required"
        exit 1
    fi
    
    # Validate email format
    if [[ ! "$USER_EMAIL" =~ ^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$ ]]; then
        print_error "Invalid email format: $USER_EMAIL"
        exit 1
    fi
    
    print_success "Domain information collected"
}

create_service_account() {
    print_step "4" "Creating Google Service Account..."
    
    echo "You'll need to create a Service Account in Google Cloud Console:"
    echo
    echo "1. Go to: https://console.cloud.google.com/"
    echo "2. Select project: $PROJECT_ID"
    echo "3. Navigate to: APIs & Services > Credentials"
    echo "4. Click 'Create Credentials' > 'Service Account'"
    echo "5. Fill in the details:"
    echo "   - Name: mail-relay-$DOMAIN"
    echo "   - Description: Service account for mail relay system"
    echo "6. Click 'Create and Continue'"
    echo "7. Skip role assignment (click 'Continue')"
    echo "8. Click 'Done'"
    echo
    
    read -p "Press Enter when you've created the service account..."
    
    print_success "Service Account creation instructions provided"
}

enable_domain_wide_delegation() {
    print_step "5" "Enabling Domain-Wide Delegation..."
    
    echo "Now you need to enable Domain-Wide Delegation:"
    echo
    echo "1. In Google Cloud Console, click on your service account"
    echo "2. Go to the 'Keys' tab"
    echo "3. Click 'Add Key' > 'Create new key'"
    echo "4. Choose 'JSON' format"
    echo "5. Download the JSON file"
    echo "6. Note the Client ID (you'll need this for the next step)"
    echo
    
    read -p "Press Enter when you've downloaded the JSON key..."
    
    print_success "Domain-Wide Delegation setup instructions provided"
}

configure_google_workspace() {
    print_step "6" "Configuring Google Workspace Admin..."
    
    echo "You need to configure Domain-Wide Delegation in Google Workspace Admin:"
    echo
    echo "1. Go to: https://admin.google.com/"
    echo "2. Navigate to: Security > Access and data control > API controls"
    echo "3. Click 'Manage Domain Wide Delegation'"
    echo "4. Click 'Add new'"
    echo "5. Fill in:"
    echo "   - Client ID: [Your Service Account Client ID]"
    echo "   - OAuth Scopes: https://mail.google.com/"
    echo "6. Click 'Authorize'"
    echo
    
    read -p "Enter the Client ID from your service account: " CLIENT_ID
    
    if [[ -z "$CLIENT_ID" ]]; then
        print_error "Client ID is required"
        exit 1
    fi
    
    print_success "Google Workspace configuration instructions provided"
}

setup_json_key() {
    print_step "7" "Setting up JSON key file..."
    
    echo "Please place your downloaded JSON key file in the service accounts directory:"
    echo "  $SERVICE_ACCOUNTS_DIR/"
    echo
    
    read -p "Enter the path to your downloaded JSON key file: " JSON_KEY_PATH
    
    if [[ ! -f "$JSON_KEY_PATH" ]]; then
        print_error "JSON key file not found: $JSON_KEY_PATH"
        exit 1
    fi
    
    # Copy and rename the file
    local target_file="$SERVICE_ACCOUNTS_DIR/${DOMAIN}-service-account.json"
    cp "$JSON_KEY_PATH" "$target_file"
    
    # Set secure permissions
    chmod 600 "$target_file"
    
    print_success "JSON key file configured: $target_file"
    
    # Extract client email for verification
    CLIENT_EMAIL=$(jq -r '.client_email' "$target_file")
    echo "Service Account Email: $CLIENT_EMAIL"
}

create_domain_config() {
    print_step "8" "Creating domain configuration..."
    
    local config_file="$CONFIG_DIR/domains/$DOMAIN.conf"
    
    cat > "$config_file" << EOF
# Domain Configuration for $DOMAIN using Service Account Authentication
# Generated by setup-service-account.sh

[domain:$DOMAIN]
enabled = true
smtp_server = smtp.gmail.com
smtp_port = 587

# Service Account Authentication
auth_method = SERVICE_ACCOUNT
service_account_file = $SERVICE_ACCOUNTS_DIR/${DOMAIN}-service-account.json
service_account_user = $USER_EMAIL

# Remote relay account (Gmail account being impersonated)
relay_account = $USER_EMAIL

# Gmail SSL/TLS settings
use_ssl = false
use_starttls = true

# Service Account: $CLIENT_EMAIL
# Project ID: $PROJECT_ID
# Client ID: $CLIENT_ID
EOF
    
    print_success "Domain configuration created: $config_file"
}

create_mailer_domain_config() {
    print_step "9" "Creating mailer domain configuration..."
    
    local mailer_domain="mailer.$DOMAIN"
    local config_file="$CONFIG_DIR/domains/$mailer_domain.conf"
    
    cat > "$config_file" << EOF
# Local Mailer Domain Configuration for $mailer_domain
# Generated by setup-service-account.sh

[domain:$mailer_domain]
enabled = true
smtp_server = localhost
smtp_port = 25
auth_method = NONE
relay_account = mail-relay@$mailer_domain
use_ssl = false
use_starttls = false

# This domain receives emails and forwards them to $DOMAIN
EOF
    
    print_success "Mailer domain configuration created: $config_file"
}

create_user_config() {
    print_step "10" "Creating user configuration..."
    
    local user_config_dir="$CONFIG_DIR/users"
    local user_file="$user_config_dir/$(echo $USER_EMAIL | sed 's/@/_at_/')"
    
    mkdir -p "$user_config_dir"
    
    cat > "$user_file" << EOF
# User Configuration for $USER_EMAIL
# Generated by setup-service-account.sh

[user:$USER_EMAIL]
enabled = true
domain = $DOMAIN
can_send_from = true
can_send_to = true
template_address = false
allowed_recipients = *@$DOMAIN
allowed_domains = $DOMAIN
EOF
    
    print_success "User configuration created: $user_file"
}

create_mapping_config() {
    print_step "11" "Creating address mapping configuration..."
    
    local mappings_dir="$CONFIG_DIR/mappings"
    local mapping_file="$mappings_dir/${DOMAIN}-mappings.conf"
    
    mkdir -p "$mappings_dir"
    
    cat > "$mapping_file" << EOF
# Address Mappings for $DOMAIN
# Generated by setup-service-account.sh

[mapping:contact-general]
from_pattern = contact-general@mailer.$DOMAIN
to_pattern = contact-general@$DOMAIN
smtp_account = $USER_EMAIL
domain = $DOMAIN

[mapping:contact-support]
from_pattern = contact-support@mailer.$DOMAIN
to_pattern = contact-support@$DOMAIN
smtp_account = $USER_EMAIL
domain = $DOMAIN

[mapping:contact-sales]
from_pattern = contact-sales@mailer.$DOMAIN
to_pattern = contact-sales@$DOMAIN
smtp_account = $USER_EMAIL
domain = $DOMAIN

[mapping:contact-privacy]
from_pattern = contact-privacy@mailer.$DOMAIN
to_pattern = contact-privacy@$DOMAIN
smtp_account = $USER_EMAIL
domain = $DOMAIN

[mapping:contact-legal]
from_pattern = contact-legal@mailer.$DOMAIN
to_pattern = contact-legal@$DOMAIN
smtp_account = $USER_EMAIL
domain = $DOMAIN
EOF
    
    print_success "Address mapping configuration created: $mapping_file"
}

create_main_config() {
    print_step "12" "Creating main configuration..."
    
    local main_config="$CONFIG_DIR/ssmtp-mailer.conf"
    
    cat > "$main_config" << EOF
# Main Configuration for ssmtp-mailer
# Generated by setup-service-account.sh

[global]
default_hostname = mailer.$DOMAIN
default_from = noreply@mailer.$DOMAIN
config_dir = $CONFIG_DIR
domains_dir = $CONFIG_DIR/domains
users_dir = $CONFIG_DIR/users
mappings_dir = $CONFIG_DIR/mappings
log_file = /var/log/ssmtp-mailer.log
log_level = INFO
max_connections = 10
connection_timeout = 30
read_timeout = 60
write_timeout = 60
enable_rate_limiting = true
rate_limit_per_minute = 100

# Service Account Configuration
service_accounts_dir = $SERVICE_ACCOUNTS_DIR
EOF
    
    print_success "Main configuration created: $main_config"
}

test_configuration() {
    print_step "13" "Testing configuration..."
    
    echo "Configuration files created successfully!"
    echo
    echo "Next steps:"
    echo "1. Build and install ssmtp-mailer:"
    echo "   cd $PROJECT_ROOT"
    echo "   mkdir -p build && cd build"
    echo "   cmake .. -DCMAKE_BUILD_TYPE=Release"
    echo "   make -j4"
    echo "   sudo make install"
    echo
    echo "2. Test the configuration:"
    echo "   ssmtp-mailer config"
    echo "   ssmtp-mailer test"
    echo
    echo "3. Send a test email:"
    echo "   ssmtp-mailer send \\"
    echo "     --from contact-general@mailer.$DOMAIN \\"
    echo "     --to test@example.com \\"
    echo "     --subject 'Test Email' \\"
    echo "     --body 'This is a test email from the relay system'"
    echo
    echo "4. Check logs:"
    echo "   tail -f /var/log/ssmtp-mailer.log"
    echo
    
    print_success "Configuration setup completed!"
}

main() {
    print_header
    
    echo "This script will help you set up Google Service Account authentication"
    echo "for your ssmtp-mailer system. This is more secure than App Passwords."
    echo
    
    read -p "Press Enter to continue..."
    echo
    
    check_dependencies
    create_directories
    get_domain_info
    create_service_account
    enable_domain_wide_delegation
    configure_google_workspace
    setup_json_key
    create_domain_config
    create_mailer_domain_config
    create_user_config
    create_mapping_config
    create_main_config
    test_configuration
    
    echo
    echo -e "${GREEN}🎉 Service Account setup completed successfully!${NC}"
    echo
    echo "Your mail relay system is now configured to use Google Service Account"
    echo "authentication, which is more secure and automated than App Passwords."
}

# Run main function
main "$@"
