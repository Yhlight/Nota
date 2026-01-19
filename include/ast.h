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

class StructInstantiationNode : public ExpressionNode {
public:
    std::string structName;
    std::vector<std::shared_ptr<ExpressionNode>> arguments;

    StructInstantiationNode(const std::string& name) : structName(name) {}
    void accept(ASTVisitor& visitor) override;
};

class ListNode : public ExpressionNode {
public:
    std::vector<std::shared_ptr<ExpressionNode>> elements;

    ListNode() = default;
    void accept(ASTVisitor& visitor) override;
};

class PropertyNode : public ASTNode {
public:
    std::string name;
    std::shared_ptr<ExpressionNode> value;

    PropertyNode(const std::string& name, std::shared_ptr<ExpressionNode> value)
        : name(name), value(value) {}
    void accept(ASTVisitor& visitor) override;
};

// ComponentNode now inherits from ExpressionNode to support being used as a value (e.g., Object Literal)
class ComponentNode : public ExpressionNode {
public:
    std::string type;
    std::string name; // Optional, for "Item Name"
    bool isExported = false;
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

class DelegateNode : public ASTNode {
public:
    std::vector<std::string> events; // [onClick, onHover]
    std::string target; // "parent" or "id"

    DelegateNode(const std::vector<std::string>& events, const std::string& target)
        : events(events), target(target) {}
    void accept(ASTVisitor& visitor) override;
};

class ForNode : public ComponentNode {
public:
    std::string iterator; // "item"
    std::string listName; // "list"
    std::vector<std::shared_ptr<ASTNode>> body;

    ForNode(const std::string& iterator, const std::string& listName)
        : ComponentNode("For"), iterator(iterator), listName(listName) {}
    void accept(ASTVisitor& visitor) override;
};

class StructDefinitionNode : public ASTNode {
public:
    std::string name;
    bool isExported = false;
    struct Field {
        std::string type;
        std::string name;
        std::shared_ptr<ExpressionNode> defaultValue;
    };
    std::vector<Field> fields;

    StructDefinitionNode(const std::string& name) : name(name) {}
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

class PackageNode : public ASTNode {
public:
    std::string name; // e.g., "UI" or "UI.Core"

    PackageNode(const std::string& name) : name(name) {}
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
    virtual void visit(PackageNode& node) = 0;
    virtual void visit(ComponentNode& node) = 0;
    virtual void visit(PropertyNode& node) = 0;
    virtual void visit(LiteralNode& node) = 0;
    virtual void visit(ReferenceNode& node) = 0;
    virtual void visit(BinaryExpressionNode& node) = 0;
    virtual void visit(ConditionalNode& node) = 0;
    virtual void visit(StructDefinitionNode& node) = 0;
    virtual void visit(StructInstantiationNode& node) = 0;
    virtual void visit(ListNode& node) = 0;
    virtual void visit(DelegateNode& node) = 0;
    virtual void visit(ForNode& node) = 0;
};

inline void LiteralNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
inline void ReferenceNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
inline void BinaryExpressionNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
inline void StructInstantiationNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
inline void ListNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
inline void PropertyNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
inline void ComponentNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
inline void ConditionalNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
inline void StructDefinitionNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
inline void ImportNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
inline void PackageNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
inline void ProgramNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
inline void DelegateNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
inline void ForNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
