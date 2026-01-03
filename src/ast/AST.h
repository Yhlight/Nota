#pragma once

#include "core/Token.h"
#include "ast/ASTFwd.h"
#include <string>
#include <vector>
#include <memory>
#include <optional>

// --- Value Nodes ---

// Represents a position keyword (e.g., "left", "center")
struct PositionKeywordNode {
    Token keyword;
};

// Represents the value of a 'position' property, which can have one or two keywords
struct PositionNode {
    PositionKeywordNode first;
    std::optional<PositionKeywordNode> second;
};

using LiteralValue = std::variant<std::string, double>;
struct LiteralNode {
    LiteralValue value;
    Token token;
};

// --- Expression Nodes ---
struct MemberAccessNode {
    std::unique_ptr<Expression> object;
    Token member;
};

struct IndexAccessNode {
    std::unique_ptr<Expression> object;
    std::unique_ptr<Expression> index;
    int line;
    int column;
};

struct AssignmentNode {
    std::unique_ptr<Expression> target;
    ASTValue value;
    int line;
    int column;
};

struct BinaryOperationNode {
    std::unique_ptr<Expression> left;
    Token op;
    std::unique_ptr<Expression> right;
};

using ExpressionVariant = std::variant<
    MemberAccessNode,
    IndexAccessNode,
    LiteralNode,
    BinaryOperationNode
>;

struct Expression {
    ExpressionVariant variant;
    int line;
    int column;
};

// --- Property Node ---
struct PropertyNode {
    Token name;
    ASTValue value; // ASTValue is now defined in ASTFwd.h
};

// --- Event Handler Node ---
struct EventHandlerNode {
    Token name;
    std::string body;
};

// --- State Node ---
struct StateNode {
    Token name;
    ASTValue value;
};

// --- Component Node ---
struct ComponentNode {
    Token type;
    std::vector<StateNode> states;
    std::vector<PropertyNode> properties;
    std::vector<std::unique_ptr<ComponentNode>> children;
    std::vector<AssignmentNode> assignments;
    std::vector<EventHandlerNode> event_handlers;
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
