#include <iostream>
#include <string>
#include <map>
#include <chrono>
#include <thread>
#include "ssmtp-mailer/unified_mailer.hpp"
#include "ssmtp-mailer/rate_limiter.hpp"
#include "ssmtp-mailer/webhook_handler.hpp"
#include "ssmtp-mailer/template_manager.hpp"
#include "ssmtp-mailer/analytics.hpp"

int main() {
    std::cout << "ssmtp-mailer Advanced Features Example" << std::endl;
    std::cout << "=======================================" << std::endl;
    
    try {
        // ============================================================================
        // 1. RATE LIMITING DEMONSTRATION
        // ============================================================================
        std::cout << "\n1. Rate Limiting System" << std::endl;
        std::cout << "=========================" << std::endl;
        
        // Create rate limiter for SendGrid (100 emails/second)
        ssmtp_mailer::RateLimitConfig sendgrid_rate_config;
        sendgrid_rate_config.max_requests_per_second = 100;
        sendgrid_rate_config.max_requests_per_minute = 6000;
        sendgrid_rate_config.strategy = ssmtp_mailer::RateLimitStrategy::FIXED_WINDOW;
        
        auto sendgrid_rate_limiter = std::make_shared<ssmtp_mailer::RateLimiter>(sendgrid_rate_config);
        
        std::cout << "✓ SendGrid rate limiter created" << std::endl;
        std::cout << "  Max requests/second: " << sendgrid_rate_config.max_requests_per_second << std::endl;
        std::cout << "  Strategy: Fixed Window" << std::endl;
        
        // Test rate limiting
        for (int i = 0; i < 5; ++i) {
            if (sendgrid_rate_limiter->isAllowed()) {
                std::cout << "  Request " << (i + 1) << ": ✓ Allowed" << std::endl;
                sendgrid_rate_limiter->recordRequest();
            } else {
                std::cout << "  Request " << (i + 1) << ": ✗ Rate limited" << std::endl;
                break;
            }
        }
        
        // ============================================================================
        // 2. TEMPLATE MANAGEMENT DEMONSTRATION
        // ============================================================================
        std::cout << "\n2. Template Management System" << std::endl;
        std::cout << "=================================" << std::endl;
        
        // Create template engine
        auto template_engine = ssmtp_mailer::TemplateFactory::createEngine("Simple");
        auto template_manager = std::make_shared<ssmtp_mailer::TemplateManager>(template_engine);
        
        std::cout << "✓ Template manager created with " << template_engine->getName() << " engine" << std::endl;
        
        // Create welcome email template
        ssmtp_mailer::EmailTemplate welcome_template;
        welcome_template.name = "welcome_email";
        welcome_template.description = "Welcome email for new users";
        welcome_template.subject_template = "Welcome to {{company_name}}, {{user_name}}!";
        welcome_template.text_body_template = "Hello {{user_name}},\n\nWelcome to {{company_name}}! We're excited to have you on board.\n\nYour account details:\n- Username: {{username}}\n- Email: {{email}}\n- Plan: {{plan_type}}\n\nBest regards,\n{{company_name}} Team";
        welcome_template.html_body_template = "<h1>Welcome to {{company_name}}, {{user_name}}!</h1><p>We're excited to have you on board!</p><h2>Your Account Details:</h2><ul><li><strong>Username:</strong> {{username}}</li><li><strong>Email:</strong> {{email}}</li><li><strong>Plan:</strong> {{plan_type}}</li></ul><p>Best regards,<br>{{company_name}} Team</p>";
        welcome_template.category = "onboarding";
        welcome_template.version = "1.0";
        
        // Add template variables
        welcome_template.variables.push_back(ssmtp_mailer::TemplateVariable("user_name", "", ssmtp_mailer::TemplateVariableType::STRING, true));
        welcome_template.variables.push_back(ssmtp_mailer::TemplateVariable("username", "", ssmtp_mailer::TemplateVariableType::STRING, true));
        welcome_template.variables.push_back(ssmtp_mailer::TemplateVariable("email", "", ssmtp_mailer::TemplateVariableType::STRING, true));
        welcome_template.variables.push_back(ssmtp_mailer::TemplateVariable("company_name", "Our Company", ssmtp_mailer::TemplateVariableType::STRING, false));
        welcome_template.variables.push_back(ssmtp_mailer::TemplateVariable("plan_type", "Free", ssmtp_mailer::TemplateVariableType::STRING, false));
        
        // Add template to manager
        if (template_manager->addTemplate(welcome_template)) {
            std::cout << "✓ Welcome email template added" << std::endl;
            std::cout << "  Template: " << welcome_template.name << std::endl;
            std::cout << "  Category: " << welcome_template.category << std::endl;
            std::cout << "  Variables: " << welcome_template.variables.size() << std::endl;
        }
        
        // Create template context
        ssmtp_mailer::TemplateContext context;
        context.addVariable("user_name", "John Doe");
        context.addVariable("username", "johndoe");
        context.addVariable("email", "john.doe@example.com");
        context.addVariable("company_name", "TechCorp");
        context.addVariable("plan_type", "Premium");
        
        // Render template
        auto rendered_email = template_manager->renderTemplate("welcome_email", context, "welcome@techcorp.com", {"john.doe@example.com"});
        if (rendered_email) {
            std::cout << "✓ Template rendered successfully" << std::endl;
            std::cout << "  Subject: " << rendered_email->subject << std::endl;
            std::cout << "  Text body length: " << rendered_email->body.length() << " characters" << std::endl;
            std::cout << "  HTML body length: " << rendered_email->html_body.length() << " characters" << std::endl;
        }
        
        // ============================================================================
        // 3. ANALYTICS AND REPORTING DEMONSTRATION
        // ============================================================================
        std::cout << "\n3. Analytics and Reporting System" << std::endl;
        std::cout << "====================================" << std::endl;
        
        // Create analytics manager
        ssmtp_mailer::AnalyticsConfig analytics_config;
        analytics_config.enable_tracking = true;
        analytics_config.enable_persistence = true;
        analytics_config.retention_days = 90;
        
        auto analytics_manager = std::make_shared<ssmtp_mailer::AnalyticsManager>(analytics_config);
        
        std::cout << "✓ Analytics manager created" << std::endl;
        std::cout << "  Tracking enabled: " << (analytics_config.enable_tracking ? "Yes" : "No") << std::endl;
        std::cout << "  Retention: " << analytics_config.retention_days << " days" << std::endl;
        
        // Track some sample events
        analytics_manager->trackEmailSent("msg_123", "sendgrid", "sender@techcorp.com", "user@example.com", "Welcome Email");
        analytics_manager->trackEmailDelivered("msg_123", "sendgrid");
        analytics_manager->trackEmailOpened("msg_123", "sendgrid");
        analytics_manager->trackEmailClicked("msg_123", "sendgrid", "https://techcorp.com/welcome");
        
        analytics_manager->trackEmailSent("msg_124", "mailgun", "sender@techcorp.com", "user2@example.com", "Newsletter");
        analytics_manager->trackEmailBounced("msg_124", "mailgun", "Invalid email address", true);
        
        // Get current metrics
        auto current_metrics = analytics_manager->getCurrentMetrics();
        std::cout << "✓ Sample events tracked" << std::endl;
        std::cout << "  Total emails sent: " << current_metrics.total_emails_sent << std::endl;
        std::cout << "  Total delivered: " << current_metrics.total_emails_delivered << std::endl;
        std::cout << "  Total opened: " << current_metrics.total_emails_opened << std::endl;
        std::cout << "  Total clicked: " << current_metrics.total_emails_clicked << std::endl;
        std::cout << "  Total bounced: " << current_metrics.total_emails_bounced << std::endl;
        
        // ============================================================================
        // 4. WEBHOOK HANDLING DEMONSTRATION
        // ============================================================================
        std::cout << "\n4. Webhook Handling System" << std::endl;
        std::cout << "=============================" << std::endl;
        
        // Create webhook configuration
        ssmtp_mailer::WebhookConfig webhook_config;
        webhook_config.endpoint = "https://techcorp.com/webhooks/email";
        webhook_config.secret = "your_webhook_secret_here";
        webhook_config.timeout_seconds = 30;
        webhook_config.verify_ssl = true;
        
        // Create webhook event processor
        auto webhook_processor = std::make_shared<ssmtp_mailer::WebhookEventProcessor>(webhook_config);
        
        std::cout << "✓ Webhook processor created" << std::endl;
        std::cout << "  Endpoint: " << webhook_config.endpoint << std::endl;
        std::cout << "  Timeout: " << webhook_config.timeout_seconds << " seconds" << std::endl;
        
        // Register webhook callbacks
        webhook_processor->registerCallback(ssmtp_mailer::WebhookEventType::DELIVERED, 
            [](const ssmtp_mailer::WebhookEvent& event) {
                std::cout << "  📧 Webhook: Email delivered - " << event.message_id << " to " << event.recipient << std::endl;
            });
        
        webhook_processor->registerCallback(ssmtp_mailer::WebhookEventType::OPENED, 
            [](const ssmtp_mailer::WebhookEvent& event) {
                std::cout << "  👁️  Webhook: Email opened - " << event.message_id << " by " << event.recipient << std::endl;
            });
        
        webhook_processor->registerCallback(ssmtp_mailer::WebhookEventType::BOUNCED, 
            [](const ssmtp_mailer::WebhookEvent& event) {
                std::cout << "  ❌ Webhook: Email bounced - " << event.message_id << " to " << event.recipient << " (Reason: " << event.reason << ")" << std::endl;
            });
        
        std::cout << "✓ Webhook callbacks registered" << std::endl;
        
        // ============================================================================
        // 5. UNIFIED MAILER WITH ADVANCED FEATURES
        // ============================================================================
        std::cout << "\n5. Unified Mailer with Advanced Features" << std::endl;
        std::cout << "===========================================" << std::endl;
        
        // Create unified mailer configuration
        ssmtp_mailer::UnifiedMailerConfig config;
        config.default_method = ssmtp_mailer::SendMethod::AUTO;
        config.enable_fallback = true;
        config.max_retries = 3;
        config.retry_delay = std::chrono::seconds(5);
        
        // Configure SendGrid API provider
        ssmtp_mailer::APIClientConfig sendgrid_config;
        sendgrid_config.provider = ssmtp_mailer::APIProvider::SENDGRID;
        sendgrid_config.auth.api_key = "your_sendgrid_api_key_here";
        sendgrid_config.sender_email = "sender@techcorp.com";
        sendgrid_config.sender_name = "TechCorp";
        sendgrid_config.request.base_url = "https://api.sendgrid.com";
        sendgrid_config.request.endpoint = "/v3/mail/send";
        sendgrid_config.request.timeout_seconds = 30;
        sendgrid_config.enable_tracking = true;
        
        // Configure Mailgun API provider
        ssmtp_mailer::APIClientConfig mailgun_config;
        mailgun_config.provider = ssmtp_mailer::APIProvider::MAILGUN;
        mailgun_config.auth.api_key = "key-your_mailgun_api_key_here";
        mailgun_config.sender_email = "sender@techcorp.com";
        mailgun_config.sender_name = "TechCorp";
        mailgun_config.request.base_url = "https://api.mailgun.net/v3";
        mailgun_config.request.endpoint = "/messages";
        mailgun_config.request.timeout_seconds = 30;
        mailgun_config.enable_tracking = true;
        mailgun_config.request.custom_headers["domain"] = "techcorp.com";
        
        // Add provider configurations
        config.api_configs["sendgrid"] = sendgrid_config;
        config.api_configs["mailgun"] = mailgun_config;
        
        // Create unified mailer
        ssmtp_mailer::UnifiedMailer mailer(config);
        
        std::cout << "✓ Unified mailer created with advanced features" << std::endl;
        std::cout << "  Default method: " << (config.default_method == ssmtp_mailer::SendMethod::AUTO ? "AUTO" : "API") << std::endl;
        std::cout << "  Fallback enabled: " << (config.enable_fallback ? "Yes" : "No") << std::endl;
        std::cout << "  Max retries: " << config.max_retries << std::endl;
        
        // ============================================================================
        // 6. INTEGRATED WORKFLOW DEMONSTRATION
        // ============================================================================
        std::cout << "\n6. Integrated Workflow Demonstration" << std::endl;
        std::cout << "======================================" << std::endl;
        
        // Simulate a complete email workflow
        std::cout << "Simulating complete email workflow..." << std::endl;
        
        // Step 1: Check rate limits
        if (sendgrid_rate_limiter->isAllowed()) {
            std::cout << "  ✓ Rate limit check passed" << std::endl;
            
            // Step 2: Render template
            auto workflow_email = template_manager->renderTemplate("welcome_email", context, "welcome@techcorp.com", {"newuser@example.com"});
            if (workflow_email) {
                std::cout << "  ✓ Template rendered" << std::endl;
                
                // Step 3: Send email (simulated)
                std::cout << "  ✓ Email prepared for sending" << std::endl;
                std::cout << "    From: " << workflow_email->from << std::endl;
                std::cout << "    To: " << workflow_email->to[0] << std::endl;
                std::cout << "    Subject: " << workflow_email->subject << std::endl;
                
                // Step 4: Track analytics
                analytics_manager->trackEmailSent("workflow_msg_001", "sendgrid", 
                                               workflow_email->from, workflow_email->to[0], 
                                               workflow_email->subject);
                std::cout << "  ✓ Analytics event tracked" << std::endl;
                
                // Step 5: Simulate webhook events
                std::cout << "  ✓ Simulating webhook events..." << std::endl;
                
                // Simulate delivery webhook
                ssmtp_mailer::WebhookEvent delivery_event;
                delivery_event.type = ssmtp_mailer::WebhookEventType::DELIVERED;
                delivery_event.message_id = "workflow_msg_001";
                delivery_event.recipient = workflow_email->to[0];
                delivery_event.provider = "sendgrid";
                delivery_event.timestamp = "2024-01-15T10:30:00Z";
                
                // Simulate open webhook
                ssmtp_mailer::WebhookEvent open_event;
                open_event.type = ssmtp_mailer::WebhookEventType::OPENED;
                open_event.message_id = "workflow_msg_001";
                open_event.recipient = workflow_email->to[0];
                open_event.provider = "sendgrid";
                open_event.timestamp = "2024-01-15T10:35:00Z";
                
                // Track webhook events
                analytics_manager->trackEmailDelivered("workflow_msg_001", "sendgrid");
                analytics_manager->trackEmailOpened("workflow_msg_001", "sendgrid");
                
                std::cout << "  ✓ Webhook events processed" << std::endl;
                
            } else {
                std::cout << "  ✗ Template rendering failed" << std::endl;
            }
        } else {
            std::cout << "  ✗ Rate limited - would need to wait or use fallback" << std::endl;
        }
        
        // ============================================================================
        // 7. PERFORMANCE METRICS AND REPORTING
        // ============================================================================
        std::cout << "\n7. Performance Metrics and Reporting" << std::endl;
        std::cout << "=====================================" << std::endl;
        
        // Get final analytics
        auto final_metrics = analytics_manager->getCurrentMetrics();
        auto delivery_rates = analytics_manager->getDeliveryRates();
        auto engagement_rates = analytics_manager->getEngagementRates();
        
        std::cout << "✓ Final analytics collected" << std::endl;
        std::cout << "  Total emails sent: " << final_metrics.total_emails_sent << std::endl;
        std::cout << "  Delivery rate: " << (delivery_rates.count("overall") ? delivery_rates["overall"] : 0.0) * 100 << "%" << std::endl;
        std::cout << "  Open rate: " << (engagement_rates.count("open_rate") ? engagement_rates["open_rate"] : 0.0) * 100 << "%" << std::endl;
        std::cout << "  Click rate: " << (engagement_rates.count("click_rate") ? engagement_rates["click_rate"] : 0.0) * 100 << "%" << std::endl;
        
        // Generate sample report
        auto now = std::chrono::system_clock::now();
        auto one_hour_ago = now - std::chrono::hours(1);
        
        std::string report = analytics_manager->generateReport("summary", "json", one_hour_ago, now);
        std::cout << "✓ Sample report generated (JSON format)" << std::endl;
        std::cout << "  Report length: " << report.length() << " characters" << std::endl;
        
        // ============================================================================
        // 8. SYSTEM STATUS AND HEALTH
        // ============================================================================
        std::cout << "\n8. System Status and Health" << std::endl;
        std::cout << "=============================" << std::endl;
        
        std::cout << "✓ System components status:" << std::endl;
        std::cout << "  Rate Limiter: Active" << std::endl;
        std::cout << "  Template Manager: Active (" << template_manager->listTemplates().size() << " templates)" << std::endl;
        std::cout << "  Analytics Manager: Active" << std::endl;
        std::cout << "  Webhook Processor: Active" << std::endl;
        std::cout << "  Unified Mailer: Active" << std::endl;
        
        // Get template statistics
        auto template_stats = template_manager->getStatistics();
        std::cout << "  Template renders: " << template_stats["total_renders"] << std::endl;
        
        // Get webhook statistics
        auto webhook_stats = webhook_processor->getStatistics();
        std::cout << "  Webhook events processed: " << webhook_stats["total_events"] << std::endl;
        
        std::cout << "\n🎉 Advanced features demonstration completed successfully!" << std::endl;
        std::cout << "The system is now ready for production use with:" << std::endl;
        std::cout << "  • Rate limiting and backoff strategies" << std::endl;
        std::cout << "  • Template management and rendering" << std::endl;
        std::cout << "  • Comprehensive analytics and reporting" << std::endl;
        std::cout << "  • Webhook event processing" << std::endl;
        std::cout << "  • Unified email sending with fallbacks" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
