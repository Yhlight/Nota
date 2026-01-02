#pragma once

#include "ast/AST.h"
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

class CodeGenerator {
public:
    std::string generate(const RootNode& root);

private:
    void generate_component(const ComponentNode& component, const ComponentNode* parent, std::stringstream& html_builder, const std::vector<const PropertyNode*>* overridden_properties = nullptr);
    void generate_item_component(const ItemNode& item_def, const ComponentNode& instance, const ComponentNode* parent, std::stringstream& html_builder);
    void generate_css_for_component(const ComponentNode& component, const std::string& class_name, const ComponentNode* parent, const std::vector<const PropertyNode*>* overridden_properties);

    std::string to_css_property(const std::string& nota_property);
    std::string to_css_value(const ASTValue& value, const std::string& property_name);

    std::string get_unique_class_name(const ComponentNode& component);

    std::unordered_map<std::string, const ItemNode*> item_definitions_;
    std::stringstream css_stream_;
    std::stringstream html_stream_;
    std::unordered_map<const ComponentNode*, std::string> class_map_;
    int class_counter_ = 0;
};
