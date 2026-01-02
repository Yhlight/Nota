#pragma once

#include "ast/AST.h"
#include <string>
#include <sstream>
#include <unordered_map>

class CodeGenerator {
public:
    std::string generate(const RootNode& root);

private:
    void visit(const ComponentNode& component, const ComponentNode* parent, std::stringstream& html_builder);
    void generate_css_for_component(const ComponentNode& component, const std::string& class_name, const ComponentNode* parent);

    std::string to_css_property(const std::string& nota_property);
    std::string to_css_value(const ASTValue& value, const std::string& property_name);

    std::string get_unique_class_name(const ComponentNode& component);

    std::stringstream css_stream_;
    std::stringstream html_stream_;
    std::unordered_map<const ComponentNode*, std::string> class_map_;
    int class_counter_ = 0;
};
