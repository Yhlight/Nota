#include "ExpressionEvaluator.h"
#include <variant>

ExpressionEvaluator::ExpressionEvaluator(std::unordered_map<const ComponentNode*, ComponentNode*>& component_map, ComponentNode* root)
    : component_map_(component_map), root_component_(root) {}

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
                return root_component_;
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
        return evaluate_expression(**expr_ptr, table, root_component_);
    }
    // Return a default-constructed LiteralNode if evaluation fails.
    return LiteralNode{};
}

ASTValue ExpressionEvaluator::evaluate_expression(const Expression& expression, SymbolTable& table, ComponentNode* current_component) {
    if (auto* member_access = std::get_if<MemberAccessNode>(&expression.variant)) {
        ComponentNode* object = evaluate_target(*member_access->object, table, current_component);
        if (object) {
            for (const auto& prop : object->properties) {
                if (prop.name.text == member_access->member.text) {
                    if (auto* literal = std::get_if<LiteralNode>(&prop.value)) {
                        return *literal;
                    }
                }
            }
        }
    }
    if (auto* literal = std::get_if<LiteralNode>(&expression.variant)) {
        return *literal;
    }
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
