#pragma once

#include "ast/AST.h"
#include "codegen/ExpressionEvaluator.h"
#include "core/Error.h"
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

class CodeGenerator {
public:
    std::string generate(const RootNode& root);
    const std::vector<CompilerError>& errors() const { return errors_; }

private:
    void generate_component(const ComponentNode& component, const ComponentNode* parent, std::stringstream& html_builder, const std::vector<const PropertyNode*>* overridden_properties = nullptr);
    void generate_item_component(const ItemNode& item_def, const ComponentNode& instance, const ComponentNode* parent, std::stringstream& html_builder);
    void apply_assignments(const RootNode& root);
    std::string get_or_create_class_for_component(const ComponentNode& component, const ComponentNode* parent, const std::vector<const PropertyNode*>* overridden_properties);
    std::string generate_css_properties_string(const ComponentNode& component, const ComponentNode* parent, const std::vector<const PropertyNode*>* overridden_properties);

    using EvaluatedValue = std::variant<double, std::string, PositionNode>;
    std::string to_css_property(const std::string& nota_property);
    std::optional<EvaluatedValue> to_css_value(const ASTValue& value, const std::string& property_name, ComponentNode* current_component);

    std::unique_ptr<ExpressionEvaluator> evaluator_;
    std::unordered_map<std::string, const ItemNode*> item_definitions_;
    std::unordered_map<const ComponentNode*, ComponentNode*> component_map_;
    void generate_scripts(const RootNode& root);
    void find_and_generate_scripts(const ComponentNode& component);
    std::string get_or_assign_id(const ComponentNode& component);

    std::stringstream css_stream_;
    std::stringstream html_stream_;
    std::stringstream script_stream_;
    std::unordered_map<const ComponentNode*, std::string> class_map_;
    std::unordered_map<const ComponentNode*, std::string> id_map_;
    std::unordered_map<std::string, std::string> style_to_class_map_;
    std::vector<CompilerError> errors_;
    int class_counter_ = 0;
    int id_counter_ = 0;
    int script_var_counter_ = 0;
};
