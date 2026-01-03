#include "SemanticAnalyzer.h"
#include "ast/AST.h"
#include <iostream>
#include <regex>

SemanticAnalyzer::SemanticAnalyzer() {
    built_in_types_ = {"App", "Row", "Col", "Rect", "Text"};
    // Updated list of valid events
    valid_events_ = {"onClick", "onHover", "onKeyPress", "onChange", "onSubmit"};
    valid_properties_["App"] = {"width", "height", "color"};
    valid_properties_["Row"] = {"width", "height", "color", "spacing"};
    valid_properties_["Col"] = {"width", "height", "color", "spacing"};
    valid_properties_["Rect"] = {"width", "height", "color", "radius", "border"};
    valid_properties_["Text"] = {"text", "color"};
    property_types_["width"] = {ValueType::NUMBER, ValueType::PERCENTAGE};
    property_types_["height"] = {ValueType::NUMBER, ValueType::PERCENTAGE};
    property_types_["color"] = {ValueType::COLOR, ValueType::STRING};
    property_types_["spacing"] = {ValueType::NUMBER};
    property_types_["radius"] = {ValueType::NUMBER};
    property_types_["border"] = {ValueType::STRING};
    property_types_["text"] = {ValueType::STRING};
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
        errors_.push_back({"Item '" + name + "' is already defined.", node.name.line, node.name.column});
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
        errors_.push_back({"Unknown component type '" + type_name + "'.", node.type.line, node.type.column});
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
    for (const auto& handler : node.event_handlers) {
        visit(handler, type_name);
    }
}
// Overload for EventHandlerNode to accept component type
void SemanticAnalyzer::visit(const EventHandlerNode& node, const std::string& component_type) {
    std::string event_name(node.name.text);
    if (valid_events_.find(event_name) == valid_events_.end()) {
        errors_.push_back({"Unknown event '" + event_name + "' on component '" + component_type + "'.", node.name.line, node.name.column});
    }
    // Ensure event handlers are not attached to Item definitions
    if (component_type == "Item") {
        errors_.push_back({"Event handlers are not allowed in Item definitions.", node.name.line, node.name.column});
    }
}
void SemanticAnalyzer::visit(const PropertyNode& node, std::shared_ptr<SymbolTable> table, const std::string& component_type) {
    std::string name(node.name.text);
    auto symbol = std::make_shared<Symbol>(name, SymbolType::PROPERTY, &node);
    if (!table->insert(name, symbol)) {
        errors_.push_back({"Property '" + name + "' redefined.", node.name.line, node.name.column});
    }
    if (name == "id") return;
    if (valid_properties_.count(component_type) && valid_properties_.at(component_type).find(name) == valid_properties_.at(component_type).end()) {
        errors_.push_back({"Property '" + name + "' is not a valid property of '" + component_type + "'.", node.name.line, node.name.column});
    }
    if (auto* literal = std::get_if<LiteralNode>(&node.value)) {
        ValueType type = get_type(*literal);
        if (property_types_.count(name)) {
            const auto& allowed_types = property_types_.at(name);
            if (std::find(allowed_types.begin(), allowed_types.end(), type) == allowed_types.end()) {
                errors_.push_back({"Invalid type for property '" + name + "'.", literal->token.line, literal->token.column});
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
        get_type(*literal);
    }
}
ValueType SemanticAnalyzer::get_type(const LiteralNode& node) {
    if (std::holds_alternative<double>(node.value)) {
        return ValueType::NUMBER;
    }
    if (std::holds_alternative<std::string>(node.value)) {
        const std::string& val = std::get<std::string>(node.value);
        if (val.find('%') != std::string::npos) {
            return ValueType::PERCENTAGE;
        }
        if (val.front() == '#') {
            return ValueType::COLOR;
        }
        return ValueType::STRING;
    }
    return ValueType::UNKNOWN;
}