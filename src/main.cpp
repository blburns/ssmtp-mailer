#include <iostream>
#include <string>
#include <vector>
#include "ssmtp-mailer/mailer.hpp"
#include "core/logging/logger.hpp"

void printUsage() {
    std::cout << "\nUsage: ssmtp-mailer [OPTIONS] [COMMAND] [ARGS...]" << std::endl;
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  --help, -h           Show this help message" << std::endl;
    std::cout << "  --version, -v        Show version information" << std::endl;
    std::cout << "  --config, -c FILE    Use specified configuration file" << std::endl;
    std::cout << "  --verbose, -V        Enable verbose logging" << std::endl;
    
    std::cout << "\nCommands:" << std::endl;
    std::cout << "  send                 Send an email" << std::endl;
    std::cout << "  test                 Test SMTP connection" << std::endl;
    std::cout << "  config               Show configuration status" << std::endl;
    std::cout << "  queue                Manage email queue" << std::endl;
    
    std::cout << "\nQueue Subcommands:" << std::endl;
    std::cout << "  start                Start the email processing queue" << std::endl;
    std::cout << "  stop                 Stop the email processing queue" << std::endl;
    std::cout << "  status               Show queue status" << std::endl;
    std::cout << "  add                  Add email to queue" << std::endl;
    std::cout << "  list                 List pending emails" << std::endl;
    std::cout << "  failed               List failed emails" << std::endl;
    
    std::cout << "\nExamples:" << std::endl;
    std::cout << "  ssmtp-mailer send --from user@example.com --to recipient@domain.com --subject 'Test' --body 'Hello'" << std::endl;
    std::cout << "  ssmtp-mailer queue add --from user@example.com --to recipient@domain.com --subject 'Queued' --body 'Hello'" << std::endl;
    std::cout << "  ssmtp-mailer queue start" << std::endl;
    std::cout << "  ssmtp-mailer queue status" << std::endl;
    std::cout << "  ssmtp-mailer test" << std::endl;
    std::cout << "  ssmtp-mailer --config /path/to/config.conf send --from user@example.com --to recipient@domain.com --subject 'Test' --body 'Hello'" << std::endl;
}

void printVersion() {
    std::cout << "ssmtp-mailer v0.0.1" << std::endl;
    std::cout << "Simple SMTP Mailer for Linux and macOS" << std::endl;
}

bool parseSendCommand(const std::vector<std::string>& args, std::string& from, std::string& to, 
                     std::string& subject, std::string& body, std::string& html_body) {
    from.clear();
    to.clear();
    subject.clear();
    body.clear();
    html_body.clear();
    
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--from" && i + 1 < args.size()) {
            from = args[++i];
        } else if (args[i] == "--to" && i + 1 < args.size()) {
            to = args[++i];
        } else if (args[i] == "--subject" && i + 1 < args.size()) {
            subject = args[++i];
        } else if (args[i] == "--body" && i + 1 < args.size()) {
            body = args[++i];
        } else if (args[i] == "--html" && i + 1 < args.size()) {
            html_body = args[++i];
        }
    }
    
    return !from.empty() && !to.empty() && !subject.empty() && !body.empty();
}

int main(int argc, char* argv[]) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    // Parse global options
    std::string config_file;
    bool verbose = false;
    
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--help" || args[i] == "-h") {
            printUsage();
            return 0;
        } else if (args[i] == "--version" || args[i] == "-v") {
            printVersion();
            return 0;
        } else if (args[i] == "--config" || args[i] == "-c") {
            if (i + 1 < args.size()) {
                config_file = args[++i];
            } else {
                std::cerr << "Error: --config requires a file path" << std::endl;
                return 1;
            }
        } else if (args[i] == "--verbose" || args[i] == "-V") {
            verbose = true;
        }
    }
    
    // Initialize logging
    try {
        if (verbose) {
            ssmtp_mailer::Logger::initialize("", ssmtp_mailer::LogLevel::DEBUG);
        } else {
            ssmtp_mailer::Logger::initialize("", ssmtp_mailer::LogLevel::INFO);
        }
    } catch (const std::exception& e) {
        std::cerr << "Warning: Failed to initialize logger: " << e.what() << std::endl;
    }
    
    ssmtp_mailer::Logger& logger = ssmtp_mailer::Logger::getInstance();
    logger.info("ssmtp-mailer v0.0.1 starting up");
    
    // Check if we have a command
    if (args.empty() || args[0].substr(0, 2) == "--") {
        printUsage();
        return 0;
    }
    
    std::string command = args[0];
    
    try {
        // Initialize mailer
        ssmtp_mailer::Mailer mailer(config_file);
        
        if (!mailer.isConfigured()) {
            std::cerr << "Error: Mailer not properly configured: " << mailer.getLastError() << std::endl;
            logger.error("Mailer configuration failed: " + mailer.getLastError());
            return 1;
        }
        
        logger.info("Mailer initialized successfully");
        
        if (command == "send") {
            // Parse send command arguments
            std::vector<std::string> send_args(args.begin() + 1, args.end());
            std::string from, to, subject, body, html_body;
            
            if (!parseSendCommand(send_args, from, to, subject, body, html_body)) {
                std::cerr << "Error: Invalid send command arguments" << std::endl;
                std::cerr << "Usage: send --from EMAIL --to EMAIL --subject SUBJECT --body BODY [--html HTML_BODY]" << std::endl;
                return 1;
            }
            
            logger.info("Sending email from " + from + " to " + to);
            
            ssmtp_mailer::SMTPResult result;
            if (!html_body.empty()) {
                result = mailer.sendHtml(from, to, subject, body, html_body);
            } else {
                result = mailer.send(from, to, subject, body);
            }
            
            if (result.success) {
                std::cout << "Email sent successfully!" << std::endl;
                std::cout << "Message ID: " << result.message_id << std::endl;
                logger.info("Email sent successfully with message ID: " + result.message_id);
                return 0;
            } else {
                std::cerr << "Failed to send email: " << result.error_message << std::endl;
                logger.error("Email sending failed: " + result.error_message);
                return 1;
            }
            
        } else if (command == "test") {
            logger.info("Testing SMTP connection");
            std::cout << "Testing SMTP connection..." << std::endl;
            
            if (mailer.testConnection()) {
                std::cout << "Connection test successful!" << std::endl;
                logger.info("SMTP connection test successful");
                return 0;
            } else {
                std::cerr << "Connection test failed: " << mailer.getLastError() << std::endl;
                logger.error("SMTP connection test failed: " + mailer.getLastError());
                return 1;
            }
            
        } else if (command == "config") {
            logger.info("Showing configuration status");
            std::cout << "Configuration Status:" << std::endl;
            std::cout << "  Mailer configured: " << (mailer.isConfigured() ? "Yes" : "No") << std::endl;
            
            if (!mailer.isConfigured()) {
                std::cout << "  Error: " << mailer.getLastError() << std::endl;
            } else {
                std::cout << "  Status: Ready" << std::endl;
            }
            
            return 0;
            
        } else if (command == "queue") {
            logger.info("Queue management command");
            
            if (args.size() < 2) {
                std::cerr << "Error: Queue command requires subcommand" << std::endl;
                std::cerr << "Usage: queue [start|stop|status|add|list|failed]" << std::endl;
                return 1;
            }
            
            std::string subcommand = args[1];
            
            if (subcommand == "start") {
                mailer.startQueue();
                std::cout << "Email queue started" << std::endl;
                logger.info("Email queue started");
                return 0;
                
            } else if (subcommand == "stop") {
                mailer.stopQueue();
                std::cout << "Email queue stopped" << std::endl;
                logger.info("Email queue stopped");
                return 0;
                
            } else if (subcommand == "status") {
                std::cout << "Queue Status:" << std::endl;
                std::cout << "  Running: " << (mailer.isQueueRunning() ? "Yes" : "No") << std::endl;
                std::cout << "  Size: " << mailer.getQueueSize() << std::endl;
                return 0;
                
            } else if (subcommand == "add") {
                if (args.size() < 6) {
                    std::cerr << "Error: Queue add requires --from, --to, --subject, --body" << std::endl;
                    std::cerr << "Usage: queue add --from EMAIL --to EMAIL --subject SUBJECT --body BODY" << std::endl;
                    return 1;
                }
                
                std::vector<std::string> queue_args(args.begin() + 2, args.end());
                std::string from, to, subject, body;
                
                if (!parseSendCommand(queue_args, from, to, subject, body, std::string())) {
                    std::cerr << "Error: Invalid queue add arguments" << std::endl;
                    return 1;
                }
                
                ssmtp_mailer::Email email(from, to, subject, body);
                mailer.enqueue(email);
                std::cout << "Email added to queue" << std::endl;
                logger.info("Email queued from " + from + " to " + to);
                return 0;
                
            } else if (subcommand == "list") {
                auto pending = mailer.getPendingEmails();
                std::cout << "Pending emails: " << pending.size() << std::endl;
                for (const auto& queued : pending) {
                    std::string recipient = queued.email.getAllRecipients().empty() ? "none" : queued.email.getAllRecipients()[0];
                    std::cout << "  - " << queued.email.from << " -> " << recipient 
                              << " (Priority: " << static_cast<int>(queued.priority) << ")" << std::endl;
                }
                return 0;
                
            } else if (subcommand == "failed") {
                auto failed = mailer.getFailedEmails();
                std::cout << "Failed emails: " << failed.size() << std::endl;
                for (const auto& queued : failed) {
                    std::string recipient = queued.email.getAllRecipients().empty() ? "none" : queued.email.getAllRecipients()[0];
                    std::cout << "  - " << queued.email.from << " -> " << recipient 
                              << " (Error: " << queued.last_error << ")" << std::endl;
                }
                return 0;
                
            } else {
                std::cerr << "Error: Unknown queue subcommand: " << subcommand << std::endl;
                std::cerr << "Usage: queue [start|stop|status|add|list|failed]" << std::endl;
                return 1;
            }
            
        } else {
            std::cerr << "Error: Unknown command: " << command << std::endl;
            printUsage();
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        logger.error("Application error: " + std::string(e.what()));
        return 1;
    }
    
    return 0;
}
