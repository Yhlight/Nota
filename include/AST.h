#pragma once
#include "Expr.h"
#include <string>
#include <vector>
#include <memory>

class ComponentNode; // Forward declaration

struct PropertyNode {
    std::string name;
    std::unique_ptr<Expr> value;
};

// Base class for nodes in the component tree
class Node {
public:
    virtual ~Node() = default;
    virtual std::unique_ptr<Node> clone() const = 0;
};

class ComponentNode : public Node {
public:
    std::string type;
    std::vector<PropertyNode> properties;
    std::vector<std::unique_ptr<Node>> children;

    // Helper to deep copy
    std::unique_ptr<Node> clone() const override {
        auto newNode = std::make_unique<ComponentNode>();
        newNode->type = type;

        for (const auto& prop : properties) {
             newNode->properties.push_back(PropertyNode{prop.name, prop.value->clone()});
        }

        for (const auto& child : children) {
            newNode->children.push_back(child->clone());
        }
        return newNode;
    }
};

class IfNode : public Node {
public:
    std::unique_ptr<Expr> condition;
    std::vector<std::unique_ptr<Node>> thenBranch;
    std::vector<std::unique_ptr<Node>> elseBranch;

    std::unique_ptr<Node> clone() const override {
        auto newNode = std::make_unique<IfNode>();
        newNode->condition = condition->clone();
        for (const auto& child : thenBranch) {
            newNode->thenBranch.push_back(child->clone());
        }
        for (const auto& child : elseBranch) {
            newNode->elseBranch.push_back(child->clone());
        }
        return newNode;
    }
};
