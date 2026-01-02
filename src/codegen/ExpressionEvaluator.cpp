#include "ExpressionEvaluator.h"
#include <variant>

ExpressionEvaluator::ExpressionEvaluator(std::unordered_map<const ComponentNode*, ComponentNode*>& component_map)
    : component_map_(component_map) {}

ComponentNode* ExpressionEvaluator::evaluate_target(const Expression& expression, SymbolTable& table, ComponentNode* current_component) {
    if (auto* member_access = std::get_if<MemberAccessNode>(&expression.variant)) {
        return evaluate_member_access(*member_access, table, current_component);
    }
    if (auto* index_access = std::get_if<IndexAccessNode>(&expression.variant)) {
        return evaluate_index_access(*index_access, table, current_component);
    }
    if (auto* literal = std::get_if<LiteralNode>(&expression.variant)) {
        if(auto* name = std::get_if<std::string>(&literal->value)) {
            if (*name == "this") {
                return current_component;
            }
            if (*name == "root") {
                return get_root_component();
            }
        }
    }
    return nullptr;
}

ASTValue ExpressionEvaluator::evaluate_value(const ASTValue& value, SymbolTable& table) {
    if (auto* literal = std::get_if<LiteralNode>(&value)) {
        return *literal;
    }
    if (auto* expr_ptr = std::get_if<std::unique_ptr<Expression>>(&value)) {
        const Expression& expr = **expr_ptr;
        if (auto* literal = std::get_if<LiteralNode>(&expr.variant)) {
            return *literal;
        }
    }
    // Return a default-constructed LiteralNode if evaluation fails.
    return LiteralNode{};
}


ComponentNode* ExpressionEvaluator::evaluate_member_access(const MemberAccessNode& node, SymbolTable& table, ComponentNode* current_component) {
    ComponentNode* object = evaluate_target(*node.object, table, current_component);
    if (object) {
        if (node.member.text == "children") {
            // This is a special case. The index access will handle the rest.
            return object;
        }
    }
    return nullptr;
}

ComponentNode* ExpressionEvaluator::evaluate_index_access(const IndexAccessNode& node, SymbolTable& table, ComponentNode* current_component) {
    ComponentNode* object = evaluate_target(*node.object, table, current_component);
    if (object) {
        if (auto* literal = std::get_if<LiteralNode>(&(node.index->variant))) {
            if (auto* number = std::get_if<double>(&literal->value)) {
                int index = static_cast<int>(*number);
                if (index >= 0 && index < object->children.size()) {
                    return object->children[index].get();
                }
            }
        }
    }
    return nullptr;
}

ComponentNode* ExpressionEvaluator::get_root_component() {
    for (auto const& [original, instance] : component_map_) {
        // This is a bit of a hack, assuming the root is always the first one without a parent in the map.
        // A better approach would be to pass the root explicitly.
        if (original->type.text == "App") { // A simple heuristic to find the root.
            return instance;
        }
    }
    return nullptr;
}
