#pragma once

#include <string>
#include <vector>
#include <memory>
#include "token.h"

class ASTVisitor;

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) = 0;
};

// Base class for expressions (values, math, references)
class ExpressionNode : public ASTNode {};

// Replaces ValueNode
class LiteralNode : public ExpressionNode {
public:
    Token token;
    LiteralNode(Token token) : token(token) {}
    void accept(ASTVisitor& visitor) override;
};

class ReferenceNode : public ExpressionNode {
public:
    std::string name; // "parent.width" or "id"
    ReferenceNode(const std::string& name) : name(name) {}
    void accept(ASTVisitor& visitor) override;
};

class BinaryExpressionNode : public ExpressionNode {
public:
    std::shared_ptr<ExpressionNode> left;
    Token op;
    std::shared_ptr<ExpressionNode> right;

    BinaryExpressionNode(std::shared_ptr<ExpressionNode> left, Token op, std::shared_ptr<ExpressionNode> right)
        : left(left), op(op), right(right) {}
    void accept(ASTVisitor& visitor) override;
};

class PropertyNode : public ASTNode {
public:
    std::string name;
    std::shared_ptr<ExpressionNode> value; // Changed from ASTNode to ExpressionNode

    PropertyNode(const std::string& name, std::shared_ptr<ExpressionNode> value)
        : name(name), value(value) {}
    void accept(ASTVisitor& visitor) override;
};

class ComponentNode : public ASTNode {
public:
    std::string type;
    std::string name; // Optional, for "Item Name"
    std::vector<std::shared_ptr<ASTNode>> children; // Properties and nested Components

    ComponentNode(const std::string& type, const std::string& name = "")
        : type(type), name(name) {}

    void accept(ASTVisitor& visitor) override;
};

class ConditionalNode : public ComponentNode {
public:
    std::shared_ptr<ExpressionNode> condition;
    std::vector<std::shared_ptr<ASTNode>> thenBranch;
    std::vector<std::shared_ptr<ASTNode>> elseBranch;

    ConditionalNode(std::shared_ptr<ExpressionNode> condition)
        : ComponentNode("If"), condition(condition) {}

    void accept(ASTVisitor& visitor) override;
};

class ImportNode : public ASTNode {
public:
    std::string path;
    std::string alias;

    ImportNode(const std::string& path, const std::string& alias = "")
        : path(path), alias(alias) {}
    void accept(ASTVisitor& visitor) override;
};

class ProgramNode : public ASTNode {
public:
    std::vector<std::shared_ptr<ASTNode>> statements;

    void accept(ASTVisitor& visitor) override;
};

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visit(ProgramNode& node) = 0;
    virtual void visit(ImportNode& node) = 0;
    virtual void visit(ComponentNode& node) = 0;
    virtual void visit(PropertyNode& node) = 0;
    virtual void visit(LiteralNode& node) = 0;
    virtual void visit(ReferenceNode& node) = 0;
    virtual void visit(BinaryExpressionNode& node) = 0;
    virtual void visit(ConditionalNode& node) = 0;
};

inline void LiteralNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
inline void ReferenceNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
inline void BinaryExpressionNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
inline void PropertyNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
inline void ComponentNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
inline void ConditionalNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
inline void ImportNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
inline void ProgramNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
