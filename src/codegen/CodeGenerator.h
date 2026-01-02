#pragma once

#include "ast/AST.h"
#include <string>
#include <sstream>

class CodeGenerator {
public:
    std::string generate(const RootNode& root);

private:
    void visit(const ComponentNode& component, std::stringstream& html_builder);
    void generate_css_for_component(const ComponentNode& component, const std::string& class_name);

    std::string to_css_property(const std::string& nota_property);
    std::string to_css_value(const ASTValue& value);

    std::string get_unique_class_name(const std::string& base_name);

    std::stringstream css_stream_;
    std::stringstream html_stream_;
    int class_counter_ = 0;
};
