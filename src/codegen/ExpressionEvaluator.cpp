#include "ExpressionEvaluator.h"
#include "ast/ASTUtils.h"
#include <variant>

ExpressionEvaluator::ExpressionEvaluator(std::unordered_map<const ComponentNode*, ComponentNode*>& component_map, ComponentNode* root)
    : component_map_(component_map), root_component_(root) {}

ComponentNode* ExpressionEvaluator::evaluate_target(const Expression& expression, SymbolTable& table, ComponentNode* current_component) {
    return std::visit(
        [&](auto&& arg) -> ComponentNode* {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, MemberAccessNode>) {
                return evaluate_member_access(arg, table, current_component);
            } else if constexpr (std::is_same_v<T, IndexAccessNode>) {
                return evaluate_index_access(arg, table, current_component);
            } else if constexpr (std::is_same_v<T, LiteralNode>) {
                if (auto* name = std::get_if<std::string>(&arg.value)) {
                    if (*name == "this") return current_component;
                    if (*name == "root") return root_component_;
                }
            }
            errors_.push_back({ "Invalid assignment target.", expression.line, expression.column });
            return nullptr;
        },
        expression.variant
    );
}

ASTValue ExpressionEvaluator::evaluate_value(const ASTValue& value, SymbolTable& table) {
    if (auto* expr_ptr = std::get_if<std::unique_ptr<Expression>>(&value)) {
        return evaluate_expression(**expr_ptr, table, root_component_);
    }
    if (auto* literal = std::get_if<LiteralNode>(&value)) {
        return *literal;
    }
    return LiteralNode{};
}

ASTValue ExpressionEvaluator::evaluate_expression(const Expression& expression, SymbolTable& table, ComponentNode* current_component) {
    return std::visit(
        [&](auto&& arg) -> ASTValue {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, LiteralNode>) {
                return arg;
            } else if constexpr (std::is_same_v<T, MemberAccessNode>) {
                ComponentNode* object = evaluate_target(*arg.object, table, current_component);
                if (object) {
                    for (const auto& prop : object->properties) {
                        if (prop.name.text == arg.member.text) {
                            return deep_copy_ast_value(prop.value);
                        }
                    }
                    errors_.push_back({ "Property '" + std::string(arg.member.text) + "' not found.", arg.member.line, arg.member.column });
                }
            } else if constexpr (std::is_same_v<T, IndexAccessNode>) {
                ComponentNode* object = evaluate_index_access(arg, table, current_component);
                if (object) {
                    return deep_copy(*object);
                }
            }
            return LiteralNode{};
        },
        expression.variant
    );
}

ComponentNode* ExpressionEvaluator::evaluate_member_access(const MemberAccessNode& node, SymbolTable& table, ComponentNode* current_component) {
    ComponentNode* object = evaluate_target(*node.object, table, current_component);
    if (object) {
        if (node.member.text == "children") {
            return object;
        }
        errors_.push_back({ "Invalid member '" + std::string(node.member.text) + "' accessed.", node.object->line, node.object->column });
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
                } else {
                    errors_.push_back({ "Index " + std::to_string(index) + " out of bounds.", node.line, node.column });
                }
            }
        }
    }
    return nullptr;
}
