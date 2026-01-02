#pragma once

#include <memory>
#include <vector>
#include <variant>

// Forward declarations for AST nodes
struct ComponentNode;
struct PropertyNode;
struct LiteralNode;

// Forward declaration for the AST value variant
using ASTValue = std::variant<
    LiteralNode,
    std::unique_ptr<ComponentNode> // Use unique_ptr to break cyclic dependency
>;
