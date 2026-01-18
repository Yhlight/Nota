#ifndef NOTA_AST_NODES_HPP
#define NOTA_AST_NODES_HPP

#include <string>
#include <vector>
#include <memory>
#include "../Token.hpp"

// Base class for all AST nodes
struct AstNode {
    virtual ~AstNode() = default;
};

// Represents a property key-value pair, e.g., "width: 100"
struct PropertyNode : public AstNode {
    Token key;
    Token value;

    PropertyNode(Token key, Token value) : key(key), value(value) {}
};

// Represents a component, e.g., "Rect { ... }"
struct ComponentNode : public AstNode {
    Token type;
    std::vector<std::unique_ptr<PropertyNode>> properties;
    std::vector<std::unique_ptr<ComponentNode>> children;

    ComponentNode(Token type) : type(type) {}
};

#endif // NOTA_AST_NODES_HPP
