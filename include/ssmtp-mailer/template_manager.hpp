#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <functional>
#include "ssmtp-mailer/mailer.hpp"

namespace ssmtp_mailer {

/**
 * @brief Template variable type
 */
enum class TemplateVariableType {
    STRING,
    INTEGER,
    FLOAT,
    BOOLEAN,
    ARRAY,
    OBJECT
};

/**
 * @brief Template variable
 */
struct TemplateVariable {
    std::string name;
    std::string value;
    TemplateVariableType type;
    bool required;
    
    TemplateVariable() : type(TemplateVariableType::STRING), required(false) {}
    TemplateVariable(const std::string& n, const std::string& v, TemplateVariableType t = TemplateVariableType::STRING, bool req = false)
        : name(n), value(v), type(t), required(req) {}
};

/**
 * @brief Email template
 */
struct EmailTemplate {
    std::string name;
    std::string description;
    std::string subject_template;
    std::string text_body_template;
    std::string html_body_template;
    std::vector<TemplateVariable> variables;
    std::string category;
    std::string version;
    std::string created_by;
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point updated_at;
    
    EmailTemplate() = default;
};

/**
 * @brief Template rendering context
 */
struct TemplateContext {
    std::map<std::string, std::string> variables;
    std::map<std::string, std::string> metadata;
    std::string locale;
    std::string timezone;
    
    TemplateContext() = default;
    
    /**
     * @brief Add variable to context
     * @param name Variable name
     * @param value Variable value
     */
    void addVariable(const std::string& name, const std::string& value);
    
    /**
     * @brief Add metadata to context
     * @param name Metadata name
     * @param value Metadata value
     */
    void addMetadata(const std::string& name, const std::string& value);
    
    /**
     * @brief Get variable value
     * @param name Variable name
     * @param default_value Default value if not found
     * @return Variable value
     */
    std::string getVariable(const std::string& name, const std::string& default_value = "") const;
    
    /**
     * @brief Check if variable exists
     * @param name Variable name
     * @return true if exists
     */
    bool hasVariable(const std::string& name) const;
};

/**
 * @brief Template engine interface
 */
class TemplateEngine {
public:
    virtual ~TemplateEngine() = default;
    
    /**
     * @brief Render template
     * @param template_content Template content
     * @param context Rendering context
     * @return Rendered content
     */
    virtual std::string render(const std::string& template_content, const TemplateContext& context) = 0;
    
    /**
     * @brief Validate template
     * @param template_content Template content
     * @return true if valid
     */
    virtual bool validate(const std::string& template_content) = 0;
    
    /**
     * @brief Extract variables from template
     * @param template_content Template content
     * @return Vector of variable names
     */
    virtual std::vector<std::string> extractVariables(const std::string& template_content) = 0;
    
    /**
     * @brief Get engine name
     * @return Engine name
     */
    virtual std::string getName() const = 0;
};

/**
 * @brief Simple template engine (basic variable substitution)
 */
class SimpleTemplateEngine : public TemplateEngine {
public:
    std::string render(const std::string& template_content, const TemplateContext& context) override;
    bool validate(const std::string& template_content) override;
    std::vector<std::string> extractVariables(const std::string& template_content) override;
    std::string getName() const override { return "Simple"; }

private:
    std::string substituteVariables(const std::string& content, const TemplateContext& context);
    std::vector<std::string> findVariableNames(const std::string& content);
};

/**
 * @brief Handlebars-style template engine
 */
class HandlebarsTemplateEngine : public TemplateEngine {
public:
    std::string render(const std::string& template_content, const TemplateContext& context) override;
    bool validate(const std::string& template_content) override;
    std::vector<std::string> extractVariables(const std::string& template_content) override;
    std::string getName() const override { return "Handlebars"; }

private:
    std::string processHandlebars(const std::string& content, const TemplateContext& context);
    std::string processConditionals(const std::string& content, const TemplateContext& context);
    std::string processLoops(const std::string& content, const TemplateContext& context);
};

/**
 * @brief Template manager
 */
class TemplateManager {
public:
    /**
     * @brief Constructor
     * @param engine Template engine to use
     */
    explicit TemplateManager(std::shared_ptr<TemplateEngine> engine);
    
    /**
     * @brief Add template
     * @param template_data Template data
     * @return true if added successfully
     */
    bool addTemplate(const EmailTemplate& template_data);
    
    /**
     * @brief Get template by name
     * @param name Template name
     * @return Template if found, nullptr otherwise
     */
    std::shared_ptr<EmailTemplate> getTemplate(const std::string& name);
    
    /**
     * @brief Remove template
     * @param name Template name
     * @return true if removed successfully
     */
    bool removeTemplate(const std::string& name);
    
    /**
     * @brief List all templates
     * @return Vector of template names
     */
    std::vector<std::string> listTemplates() const;
    
    /**
     * @brief List templates by category
     * @param category Category name
     * @return Vector of template names
     */
    std::vector<std::string> listTemplatesByCategory(const std::string& category) const;
    
    /**
     * @brief Render template to email
     * @param template_name Template name
     * @param context Rendering context
     * @param from From address
     * @param to To address(es)
     * @return Email object
     */
    std::shared_ptr<Email> renderTemplate(const std::string& template_name, 
                                        const TemplateContext& context,
                                        const std::string& from,
                                        const std::vector<std::string>& to);
    
    /**
     * @brief Render template to email with custom recipients
     * @param template_name Template name
     * @param context Rendering context
     * @param email Email object to populate
     * @return true if successful
     */
    bool renderTemplateToEmail(const std::string& template_name, 
                             const TemplateContext& context,
                             Email& email);
    
    /**
     * @brief Validate template
     * @param template_data Template data
     * @return true if valid
     */
    bool validateTemplate(const EmailTemplate& template_data);
    
    /**
     * @brief Get template statistics
     * @return Map of statistics
     */
    std::map<std::string, int> getStatistics() const;
    
    /**
     * @brief Set template engine
     * @param engine New template engine
     */
    void setTemplateEngine(std::shared_ptr<TemplateEngine> engine);
    
    /**
     * @brief Get current template engine
     * @return Current template engine
     */
    std::shared_ptr<TemplateEngine> getTemplateEngine() const;

private:
    std::shared_ptr<TemplateEngine> engine_;
    std::map<std::string, std::shared_ptr<EmailTemplate>> templates_;
    std::map<std::string, std::vector<std::string>> templates_by_category_;
    mutable std::mutex mutex_;
    
    // Statistics
    std::atomic<int> total_templates_;
    std::atomic<int> total_renders_;
    std::atomic<int> total_errors_;
    
    bool validateTemplateContent(const EmailTemplate& template_data);
    void updateCategoryIndex(const std::string& template_name, const std::string& category);
};

/**
 * @brief Template factory
 */
class TemplateFactory {
public:
    /**
     * @brief Create template engine by name
     * @param engine_name Engine name
     * @return Template engine instance
     */
    static std::shared_ptr<TemplateEngine> createEngine(const std::string& engine_name);
    
    /**
     * @brief Get available engines
     * @return Vector of engine names
     */
    static std::vector<std::string> getAvailableEngines();
    
    /**
     * @brief Check if engine is available
     * @param engine_name Engine name
     * @return true if available
     */
    static bool isEngineAvailable(const std::string& engine_name);
};

} // namespace ssmtp_mailer
