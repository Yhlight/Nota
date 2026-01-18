#pragma once
#include "AST.h"
#include <map>
#include <string>
#include <memory>

class ComponentRegistry {
public:
    static ComponentRegistry& instance();

    void defineComponent(const std::string& name, std::unique_ptr<ComponentNode> node);
    const ComponentNode* getComponent(const std::string& name) const;
    bool hasComponent(const std::string& name) const;
    void clear();

private:
    ComponentRegistry() = default;
    std::map<std::string, std::unique_ptr<ComponentNode>> components;
};
