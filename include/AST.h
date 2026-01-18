#pragma once
#include <string>
#include <vector>
#include <memory>

struct PropertyNode {
    std::string name;
    std::string value;
};

struct ComponentNode {
    std::string type;
    std::vector<PropertyNode> properties;
    std::vector<std::unique_ptr<ComponentNode>> children;

    // Helper to deep copy
    std::unique_ptr<ComponentNode> clone() const {
        auto newNode = std::make_unique<ComponentNode>();
        newNode->type = type;
        newNode->properties = properties;
        for (const auto& child : children) {
            newNode->children.push_back(child->clone());
        }
        return newNode;
    }
};
