#include "ComponentRegistry.hpp"

ComponentRegistry& ComponentRegistry::getInstance() {
    static ComponentRegistry instance;
    return instance;
}

ComponentRegistry::ComponentRegistry() {}

void ComponentRegistry::addComponent(const std::string& name, std::unique_ptr<ComponentDefStmt> component) {
    components[name] = std::move(component);
}

const ComponentDefStmt* ComponentRegistry::getComponent(const std::string& name) const {
    auto it = components.find(name);
    if (it != components.end()) {
        return it->second.get();
    }
    return nullptr;
}

void ComponentRegistry::reset() {
    components.clear();
}
