#include "ASTUtils.h"
#include "AST.h"
#include <variant>

// Implementation of deep_copy_expression
std::unique_ptr<Expression> deep_copy_expression(const Expression& expr) {
    auto new_expr = std::make_unique<Expression>();
    new_expr->variant = std::visit(
        [&](auto&& arg) -> ExpressionVariant {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, LiteralNode>) {
                return arg; // LiteralNode is copyable
            } else if constexpr (std::is_same_v<T, MemberAccessNode>) {
                return MemberAccessNode{deep_copy_expression(*arg.object), arg.member};
            } else if constexpr (std::is_same_v<T, IndexAccessNode>) {
                return IndexAccessNode{deep_copy_expression(*arg.object), deep_copy_expression(*arg.index)};
            } else if constexpr (std::is_same_v<T, BinaryOperationNode>) {
                return BinaryOperationNode{deep_copy_expression(*arg.left), arg.op, deep_copy_expression(*arg.right)};
            }
        },
        expr.variant
    );
    return new_expr;
}

// Implementation of deep_copy_ast_value
ASTValue deep_copy_ast_value(const ASTValue& value) {
    return std::visit(
        [&](auto&& arg) -> ASTValue {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, LiteralNode>) {
                return arg; // LiteralNode is copyable
            } else if constexpr (std::is_same_v<T, PositionNode>) {
                return arg; // PositionNode is copyable
            } else if constexpr (std::is_same_v<T, std::unique_ptr<ComponentNode>>) {
                return deep_copy(*arg);
            } else if constexpr (std::is_same_v<T, std::unique_ptr<Expression>>) {
                return deep_copy_expression(*arg);
            }
            // Add a default case to handle unexpected types
            return LiteralNode{};
        },
        value
    );
}

// Implementation of deep_copy for ComponentNode
std::unique_ptr<ComponentNode> deep_copy(const ComponentNode& node) {
    auto new_node = std::make_unique<ComponentNode>();
    new_node->type = node.type;
    for (const auto& prop : node.properties) {
        PropertyNode new_prop;
        new_prop.name = prop.name;
        new_prop.value = deep_copy_ast_value(prop.value);
        new_node->properties.push_back(std::move(new_prop));
    }
    for (const auto& child : node.children) {
        new_node->children.push_back(deep_copy(*child));
    }
    for (const auto& assignment : node.assignments) {
        AssignmentNode new_assignment;
        new_assignment.target = deep_copy_expression(*assignment.target);
        new_assignment.value = deep_copy_ast_value(assignment.value);
        new_node->assignments.push_back(std::move(new_assignment));
    }
    for (const auto& handler : node.event_handlers) {
        EventHandlerNode new_handler;
        new_handler.name = handler.name;
        new_handler.value = deep_copy_ast_value(handler.value);
        new_node->event_handlers.push_back(std::move(new_handler));
    }
    for (const auto& decl : node.state_declarations) {
        StateDeclarationNode new_decl;
        new_decl.name = decl.name;
        new_decl.initial_value = deep_copy_ast_value(decl.initial_value);
        new_node->state_declarations.push_back(std::move(new_decl));
    }
    return new_node;
}
