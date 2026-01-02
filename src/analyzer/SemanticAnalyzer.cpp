#include "SemanticAnalyzer.h"
#include "ast/AST.h"
#include <iostream>

SemanticAnalyzer::SemanticAnalyzer() = default;

bool SemanticAnalyzer::analyze(const RootNode& ast) {
    auto global_scope = std::make_shared<SymbolTable>();

    for (const auto& item_def : ast.item_definitions) {
        visit(*item_def, global_scope);
    }

    if (ast.root_component) {
        visit(*ast.root_component, global_scope);
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
        visit(prop, local_scope);
    }

    for (const auto& child : node.children) {
        if (child) {
            visit(*child, local_scope);
        }
    }
}

void SemanticAnalyzer::visit(const ComponentNode& node, std::shared_ptr<SymbolTable> table) {
    std::string type_name(node.type.text);
    if (table->lookup(type_name) == nullptr) {
        // It's not a defined Item, so it must be a built-in component.
        // For now, we assume all non-Item components are valid.
    }

    auto local_scope = std::make_shared<SymbolTable>(table);
    for (const auto& prop : node.properties) {
        visit(prop, local_scope);
    }

    for (const auto& child : node.children) {
        if (child) {
            visit(*child, local_scope);
        }
    }
}

void SemanticAnalyzer::visit(const PropertyNode& node, std::shared_ptr<SymbolTable> table) {
    std::string name(node.name.text);
    auto symbol = std::make_shared<Symbol>(name, SymbolType::PROPERTY, &node);
    if (!table->insert(name, symbol)) {
        errors_.push_back("Property '" + name + "' redefined.");
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
