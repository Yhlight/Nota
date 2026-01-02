#pragma once

#include "core/Token.h"
#include "ast/ASTFwd.h"
#include <string>
#include <vector>
#include <memory>

// --- Value Nodes ---
using LiteralValue = std::variant<std::string, double>;
struct LiteralNode {
    LiteralValue value;
    Token token;
};

// --- Property Node ---
struct PropertyNode {
    Token name;
    ASTValue value; // ASTValue is now defined in ASTFwd.h
};

// --- Component Node ---
struct ComponentNode {
    Token type;
    std::vector<PropertyNode> properties;
    std::vector<std::unique_ptr<ComponentNode>> children;
};

// --- Item Definition Node ---
struct ItemNode {
    Token name;
    std::vector<PropertyNode> properties;
    std::vector<std::unique_ptr<ComponentNode>> children;
};

// --- Root of the AST ---
struct RootNode {
    std::vector<std::unique_ptr<ItemNode>> item_definitions;
    std::unique_ptr<ComponentNode> root_component;
};
