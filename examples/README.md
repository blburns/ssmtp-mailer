# ssmtp-mailer Examples

This directory contains comprehensive examples demonstrating how to use the ssmtp-mailer library with different API providers and features.

## 📁 Example Programs

### **1. Basic API Example** (`api_example.cpp`)
**Purpose**: Demonstrates basic SendGrid API integration
**Features**:
- Simple email composition and sending
- Basic error handling
- Statistics tracking
- Connection testing

**Usage**:
```bash
# Compile
g++ -std=c++17 -I../include -L../lib -lssmtp-mailer api_example.cpp -o api_example

# Run
./api_example
```

### **2. SendGrid Example** (`sendgrid_example.cpp`)
**Purpose**: Complete SendGrid API integration example
**Features**:
- Full SendGrid v3 API support
- HTML and text email composition
- Email tracking configuration
- Batch sending demonstration
- Comprehensive error handling

**Usage**:
```bash
# Compile
g++ -std=c++17 -I../include -L../lib -lssmtp-mailer sendgrid_example.cpp -o sendgrid_example

# Run
./sendgrid_example
```

### **3. Mailgun Example** (`mailgun_example.cpp`)
**Purpose**: Complete Mailgun API integration example
**Features**:
- Mailgun API v3 integration
- Form-encoded request building
- Domain configuration
- Basic authentication
- Batch sending support

**Usage**:
```bash
# Compile
g++ -std=c++17 -I../include -L../lib -lssmtp-mailer mailgun_example.cpp -o mailgun_example

# Run
./mailgun_example
```

### **4. Amazon SES Example** (`amazon_ses_example.cpp`)
**Purpose**: Complete Amazon SES API integration example
**Features**:
- AWS SES v2 API integration
- JSON request building
- Region configuration
- Configuration set support
- Email tagging for analytics

**Usage**:
```bash
# Compile
g++ -std=c++17 -I../include -L../lib -lssmtp-mailer amazon_ses_example.cpp -o amazon_ses_example

# Run
./amazon_ses_example
```

### **5. Multi-Provider Example** (`multi_provider_example.cpp`)
**Purpose**: Demonstrates using multiple API providers together
**Features**:
- Unified mailer configuration
- Multiple provider setup
- Automatic method selection
- Fallback mechanisms
- Provider comparison and statistics

**Usage**:
```bash
# Compile
g++ -std=c++17 -I../include -L../lib -lssmtp-mailer multi_provider_example.cpp -o multi_provider_example

# Run
./multi_provider_example
```

## 🚀 Quick Start

### **Prerequisites**
1. **ssmtp-mailer library** built and installed
2. **API credentials** for your chosen provider(s)
3. **C++17 compatible compiler** (GCC 7+, Clang 5+, MSVC 2017+)

### **1. Choose Your Provider**
- **SendGrid**: Best for marketing emails, high deliverability
- **Mailgun**: Best for transactional emails, developer-friendly
- **Amazon SES**: Best for high-volume, cost-effective sending

### **2. Get API Credentials**
- **SendGrid**: [Create account](https://sendgrid.com) → Settings → API Keys
- **Mailgun**: [Create account](https://mailgun.com) → Dashboard → API Keys
- **Amazon SES**: [AWS Console](https://console.aws.amazon.com/ses/) → Create IAM user

### **3. Configure Your Example**
Edit the example file and replace placeholder credentials:
```cpp
// Replace these with your actual credentials
config.auth.api_key = "your_actual_api_key_here";
config.sender_email = "your-verified-sender@yourdomain.com";
```

### **4. Build and Run**
```bash
# Navigate to examples directory
cd examples

# Build your chosen example
g++ -std=c++17 -I../include -L../lib -lssmtp-mailer your_example.cpp -o your_example

# Run the example
./your_example
```

## ⚙️ Configuration

### **API Configuration File**
Create `api-config.conf` in your working directory:

```ini
[api:sendgrid]
enabled = true
provider = SENDGRID
api_key = your_sendgrid_api_key_here
sender_email = your-verified-sender@yourdomain.com
sender_name = Your Company Name

[api:mailgun]
enabled = true
provider = MAILGUN
api_key = key-your_mailgun_api_key_here
sender_email = your-verified-sender@yourdomain.com
domain = yourdomain.com

[api:amazon-ses]
enabled = true
provider = AMAZON_SES
api_key = your_aws_access_key_id
api_secret = your_aws_secret_access_key
sender_email = your-verified-sender@yourdomain.com
region = us-east-1
```

### **Environment Variables**
Set these for secure credential management:
```bash
export SENDGRID_API_KEY="your_key_here"
export MAILGUN_API_KEY="your_key_here"
export AWS_ACCESS_KEY_ID="your_key_here"
export AWS_SECRET_ACCESS_KEY="your_secret_here"
```

## 🔧 Building Examples

### **CMake Build (Recommended)**
```bash
# From project root
mkdir build && cd build
cmake ..
make examples
```

### **Manual Build**
```bash
# Set include and library paths
export CPLUS_INCLUDE_PATH="../include:$CPLUS_INCLUDE_PATH"
export LIBRARY_PATH="../lib:$LIBRARY_PATH"
export LD_LIBRARY_PATH="../lib:$LD_LIBRARY_PATH"

# Compile
g++ -std=c++17 -I../include -L../lib -lssmtp-mailer your_example.cpp -o your_example
```

### **Dependencies**
- **libcurl**: HTTP client functionality
- **OpenSSL**: SSL/TLS support
- **jsoncpp**: JSON parsing (for future enhancements)

## 📧 Email Composition

### **Basic Email**
```cpp
ssmtp_mailer::Email email;
email.from = "sender@domain.com";
email.to.push_back("recipient@domain.com");
email.subject = "Subject";
email.body = "Text body";
email.html_body = "<h1>HTML body</h1>";
```

### **Multiple Recipients**
```cpp
email.to.push_back("recipient1@domain.com");
email.to.push_back("recipient2@domain.com");
email.cc.push_back("cc@domain.com");
email.bcc.push_back("bcc@domain.com");
```

### **Attachments**
```cpp
email.attachments.push_back("/path/to/file.pdf");
email.attachments.push_back("/path/to/image.jpg");
```

## 🎯 Provider-Specific Features

### **SendGrid**
- **HTML templates** with dynamic content
- **Email tracking** (opens, clicks)
- **Unsubscribe management**
- **Advanced analytics**

### **Mailgun**
- **Webhook support** for delivery events
- **Domain verification** and management
- **Detailed logging** and monitoring
- **Rate limiting** configuration

### **Amazon SES**
- **AWS integration** (CloudWatch, SNS)
- **Configuration sets** for tracking
- **Reputation management**
- **Cost optimization**

## 🚨 Error Handling

### **Common Errors**
1. **Authentication failed**: Check API keys and credentials
2. **Rate limiting**: Reduce sending frequency
3. **Invalid sender**: Verify sender email address
4. **Domain not verified**: Complete domain verification

### **Error Handling Example**
```cpp
auto result = client->sendEmail(email);
if (!result.success) {
    std::cerr << "Failed: " << result.error_message << std::endl;
    std::cerr << "HTTP Code: " << result.http_code << std::endl;
    
    // Check if we should retry
    if (result.retry_count < max_retries) {
        // Implement retry logic
    }
}
```

## 📊 Monitoring and Analytics

### **Statistics Tracking**
```cpp
auto stats = mailer.getStatistics();
std::cout << "API success: " << stats["api_success"] << std::endl;
std::cout << "API failures: " << stats["api_failure"] << std::endl;
std::cout << "Fallbacks: " << stats["fallbacks"] << std::endl;
```

### **Provider Analytics**
- **SendGrid**: Dashboard → Activity → Mail
- **Mailgun**: Dashboard → Logs → Events
- **Amazon SES**: Console → Sending Statistics

## 🔒 Security Best Practices

1. **Store credentials securely** (environment variables, key management)
2. **Verify sender addresses** with providers
3. **Use HTTPS** for all API communications
4. **Monitor usage** for anomalies
5. **Rotate API keys** periodically

## 🧪 Testing

### **Test Programs**
- **`test_api_client`**: Basic functionality testing
- **`test_all_providers`**: Comprehensive provider testing

### **Running Tests**
```bash
# Build tests
make test

# Run specific test
./test_all_providers
```

## 📚 Additional Resources

- **API Documentation**: See `docs/api/README.md`
- **Configuration Guide**: See `config/api-config.conf.example`
- **Implementation Summary**: See `API_IMPLEMENTATION_SUMMARY.md`
- **Provider Documentation**:
  - [SendGrid API](https://sendgrid.com/docs/api-reference/)
  - [Mailgun API](https://documentation.mailgun.com/en/latest/api_reference.html)
  - [Amazon SES API](https://docs.aws.amazon.com/ses/latest/APIReference/)

## 🤝 Contributing

To add new examples or improve existing ones:

1. **Follow the existing code style**
2. **Include comprehensive error handling**
3. **Add detailed comments** explaining functionality
4. **Test with real API credentials** (use test accounts)
5. **Update this README** with new examples

## 📞 Support

For issues and questions:

1. **Check provider documentation** first
2. **Review error logs** and response codes
3. **Test with simple examples** to isolate issues
4. **Contact support** with detailed error information

---

**Happy email sending! 🚀📧**
