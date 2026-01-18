#pragma once
#include <string>
#include <vector>
#include <memory>
#include <map>

struct PropertyNode {
    std::string name;
    std::string value; // Stored as string for now
};

struct ComponentNode {
    std::string type;
    std::vector<PropertyNode> properties;
    std::vector<std::unique_ptr<ComponentNode>> children;
};
