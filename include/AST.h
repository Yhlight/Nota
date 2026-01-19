#pragma once
#include "Expr.h"
#include <string>
#include <vector>
#include <memory>

struct PropertyNode {
    std::string name;
    std::unique_ptr<Expr> value;
};

struct ComponentNode {
    std::string type;
    std::vector<PropertyNode> properties;
    std::vector<std::unique_ptr<ComponentNode>> children;

    // Helper to deep copy
    std::unique_ptr<ComponentNode> clone() const {
        auto newNode = std::make_unique<ComponentNode>();
        newNode->type = type;
        // Properties need deep copy of Expr.
        // Since Expr is polymorphic, we need a clone method on Expr or re-parse.
        // For simplicity now, we might need to implement clone() in Expr.
        // Or, for this Conductor, assume templates are static or parsed again?
        // Actually, Parser instantiates by cloning. We MUST implement clone for Expr.

        for (const auto& prop : properties) {
             newNode->properties.push_back(PropertyNode{prop.name, prop.value->clone()});
        }

        for (const auto& child : children) {
            newNode->children.push_back(child->clone());
        }
        return newNode;
    }
};
