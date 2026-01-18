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

class ValueNode : public ASTNode {
public:
    Token token;
    ValueNode(Token token) : token(token) {}
    void accept(ASTVisitor& visitor) override;
};

class PropertyNode : public ASTNode {
public:
    std::string name;
    std::shared_ptr<ASTNode> value;

    PropertyNode(const std::string& name, std::shared_ptr<ASTNode> value)
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

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visit(ComponentNode& node) = 0;
    virtual void visit(PropertyNode& node) = 0;
    virtual void visit(ValueNode& node) = 0;
};

inline void ValueNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
inline void PropertyNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
inline void ComponentNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
