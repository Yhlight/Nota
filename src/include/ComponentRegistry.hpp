#ifndef NOTA_COMPONENT_REGISTRY_HPP
#define NOTA_COMPONENT_REGISTRY_HPP

#include "AST.hpp"
#include <string>
#include <unordered_map>
#include <memory>

class ComponentRegistry {
public:
    static ComponentRegistry& getInstance();

    void addComponent(const std::string& name, std::unique_ptr<ComponentDefStmt> component);
    const ComponentDefStmt* getComponent(const std::string& name) const;
    void reset();

private:
    ComponentRegistry();
    std::unordered_map<std::string, std::unique_ptr<ComponentDefStmt>> components;
};

#endif // NOTA_COMPONENT_REGISTRY_HPP
