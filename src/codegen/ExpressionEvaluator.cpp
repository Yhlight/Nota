#include "ExpressionEvaluator.h"
#include "ast/ASTUtils.h"
#include <variant>

ExpressionEvaluator::ExpressionEvaluator(std::unordered_map<const ComponentNode*, ComponentNode*>& component_map, ComponentNode* root)
    : component_map_(component_map), root_component_(root) {}

void ExpressionEvaluator::build_parent_map(ComponentNode* component, ComponentNode* parent) {
    if (parent) {
        parent_map_[component] = parent;
    }
    for (auto& child : component->children) {
        build_parent_map(child.get(), component);
    }
}

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
                    if (*name == "parent") {
                        if (parent_map_.count(current_component)) {
                            return parent_map_.at(current_component);
                        }
                        return nullptr; // No parent
                    }
                }
            }
            errors_.push_back({ "Invalid assignment target.", expression.line, expression.column });
            return nullptr;
        },
        expression.variant
    );
}

ASTValue ExpressionEvaluator::evaluate_value(const ASTValue& value, SymbolTable& table, ComponentNode* current_component) {
    if (auto* expr_ptr = std::get_if<std::unique_ptr<Expression>>(&value)) {
        return evaluate_expression(**expr_ptr, table, current_component);
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
                            return evaluate_value(prop.value, table, object);
                        }
                    }
                    errors_.push_back({ "Property '" + std::string(arg.member.text) + "' not found.", arg.member.line, arg.member.column });
                }
            } else if constexpr (std::is_same_v<T, IndexAccessNode>) {
                ComponentNode* object = evaluate_index_access(arg, table, current_component);
                if (object) {
                    return deep_copy(*object);
                }
            } else if constexpr (std::is_same_v<T, BinaryOperationNode>) {
                return evaluate_binary_operation(arg, table, current_component);
            }
            return LiteralNode{};
        },
        expression.variant
    );
}

ASTValue ExpressionEvaluator::evaluate_binary_operation(const BinaryOperationNode& node, SymbolTable& table, ComponentNode* current_component) {
    ASTValue left_val = evaluate_expression(*node.left, table, current_component);
    ASTValue right_val = evaluate_expression(*node.right, table, current_component);

    auto* left_literal = std::get_if<LiteralNode>(&left_val);
    auto* right_literal = std::get_if<LiteralNode>(&right_val);

    if (!left_literal || !right_literal) {
        errors_.push_back({"Binary operation operands must be literals.", node.op.line, node.op.column});
        return LiteralNode{0.0, node.op};
    }

    auto* left_num = std::get_if<double>(&left_literal->value);
    auto* right_num = std::get_if<double>(&right_literal->value);

    if (!left_num || !right_num) {
        errors_.push_back({"Binary operation operands must be numbers.", node.op.line, node.op.column});
        return LiteralNode{0.0, node.op};
    }

    double result = 0.0;
    switch (node.op.type) {
        case TokenType::PLUS:
            result = *left_num + *right_num;
            break;
        case TokenType::MINUS:
            result = *left_num - *right_num;
            break;
        case TokenType::STAR:
            result = *left_num * *right_num;
            break;
        case TokenType::SLASH:
            if (*right_num == 0) {
                errors_.push_back({"Division by zero.", node.op.line, node.op.column});
                return LiteralNode{0.0, node.op};
            }
            result = *left_num / *right_num;
            break;
        default:
            errors_.push_back({"Unsupported binary operator.", node.op.line, node.op.column});
            return LiteralNode{0.0, node.op};
    }
    return LiteralNode{result, node.op};
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
