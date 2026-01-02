#pragma once

#include <memory>
#include <vector>
#include <variant>

// Forward declarations for AST nodes
struct ComponentNode;
struct PropertyNode;
struct LiteralNode;
struct MemberAccessNode;
struct IndexAccessNode;
struct AssignmentNode;
struct Expression;

// Forward declaration for the AST value variant
using ASTValue = std::variant<
    LiteralNode,
    std::unique_ptr<ComponentNode>,
    std::unique_ptr<Expression>
>;
