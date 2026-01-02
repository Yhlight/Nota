#include "SemanticAnalyzer.h"
#include "ast/AST.h"
#include <iostream>

SemanticAnalyzer::SemanticAnalyzer() {
    built_in_types_ = {"App", "Row", "Col", "Rect", "Text"};
    valid_properties_["App"] = {"width", "height", "color"};
    valid_properties_["Row"] = {"width", "height", "color", "spacing"};
    valid_properties_["Col"] = {"width", "height", "color", "spacing"};
    valid_properties_["Rect"] = {"width", "height", "color", "radius", "border"};
    valid_properties_["Text"] = {"text", "color"};
}

bool SemanticAnalyzer::analyze(const RootNode& ast) {
    auto global_scope = std::make_shared<SymbolTable>();

    for (const auto& item_def : ast.item_definitions) {
        visit(*item_def, global_scope);
    }

    if (ast.root_component) {
        visit(*ast.root_component, global_scope, "");
    }

    return errors_.empty();
}

void SemanticAnalyzer::visit(const ItemNode& node, std::shared_ptr<SymbolTable> table) {
    std::string name(node.name.text);
    auto symbol = std::make_shared<Symbol>(name, SymbolType::COMPONENT, &node);
    if (!table->insert(name, symbol)) {
        errors_.push_back("Item '" + name + "' is already defined.");
    }

    auto local_scope = std::make_shared<SymbolTable>(table);
    for (const auto& prop : node.properties) {
        visit(prop, local_scope, std::string(node.name.text));
    }

    for (const auto& child : node.children) {
        if (child) {
            visit(*child, local_scope, std::string(node.name.text));
        }
    }
}

void SemanticAnalyzer::visit(const ComponentNode& node, std::shared_ptr<SymbolTable> table, const std::string& parent_type) {
    std::string type_name(node.type.text);
    if (built_in_types_.find(type_name) == built_in_types_.end() && table->lookup(type_name) == nullptr) {
        errors_.push_back("Unknown component type '" + type_name + "'.");
    }

    auto local_scope = std::make_shared<SymbolTable>(table);
    for (const auto& prop : node.properties) {
        visit(prop, local_scope, type_name);
    }

    for (const auto& child : node.children) {
        if (child) {
            visit(*child, local_scope, type_name);
        }
    }

    for (const auto& assignment : node.assignments) {
        visit(assignment, local_scope);
    }
}

void SemanticAnalyzer::visit(const PropertyNode& node, std::shared_ptr<SymbolTable> table, const std::string& component_type) {
    std::string name(node.name.text);
    auto symbol = std::make_shared<Symbol>(name, SymbolType::PROPERTY, &node);
    if (!table->insert(name, symbol)) {
        errors_.push_back("Property '" + name + "' redefined.");
    }

    if (name == "id") return;

    if (valid_properties_.count(component_type) && valid_properties_.at(component_type).find(name) == valid_properties_.at(component_type).end()) {
        errors_.push_back("Property '" + name + "' is not a valid property of '" + component_type + "'.");
    }

    if (name == "width" || name == "height") {
        if (auto* literal = std::get_if<LiteralNode>(&node.value)) {
            if (auto* str_val = std::get_if<std::string>(&literal->value)) {
                if (str_val->find('%') == std::string::npos && !std::isdigit((*str_val)[0])) {
                     errors_.push_back("Invalid value for property '" + name + "'. Expected number or percentage.");
                }
            }
        }
    }
}

void SemanticAnalyzer::visit(const AssignmentNode& node, std::shared_ptr<SymbolTable> table) {
    visit(*node.target, table);
    if (auto* expr = std::get_if<std::unique_ptr<Expression>>(&node.value)) {
        visit(**expr, table);
    }
}

void SemanticAnalyzer::visit(const Expression& node, std::shared_ptr<SymbolTable> table) {
    if (auto* member_access = std::get_if<MemberAccessNode>(&node.variant)) {
        visit(*member_access->object, table);
    } else if (auto* index_access = std::get_if<IndexAccessNode>(&node.variant)) {
        visit(*index_access->object, table);
        visit(*index_access->index, table);
    } else if (auto* literal = std::get_if<LiteralNode>(&node.variant)) {
        // Nothing to do for literals.
    }
}
