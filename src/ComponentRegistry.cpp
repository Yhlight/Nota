#include "ComponentRegistry.h"

ComponentRegistry& ComponentRegistry::instance() {
    static ComponentRegistry instance;
    return instance;
}

void ComponentRegistry::defineComponent(const std::string& name, std::unique_ptr<ComponentNode> node) {
    components[name] = std::move(node);
}

const ComponentNode* ComponentRegistry::getComponent(const std::string& name) const {
    auto it = components.find(name);
    if (it != components.end()) {
        return it->second.get();
    }
    return nullptr;
}

bool ComponentRegistry::hasComponent(const std::string& name) const {
    return components.find(name) != components.end();
}

void ComponentRegistry::clear() {
    components.clear();
}
