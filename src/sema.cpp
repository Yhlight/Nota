#include "sema.h"
#include <iostream>

void ComponentRegistry::registerComponent(const std::string& name, std::shared_ptr<ComponentNode> definition) {
    components[name] = definition;
}

std::shared_ptr<ComponentNode> ComponentRegistry::getComponent(const std::string& name) const {
    auto it = components.find(name);
    if (it != components.end()) {
        return it->second;
    }
    return nullptr;
}

bool ComponentRegistry::hasComponent(const std::string& name) const {
    return components.find(name) != components.end();
}

void SemanticAnalyzer::analyze(ProgramNode& root) {
    root.accept(*this);
}

void SemanticAnalyzer::visit(ProgramNode& node) {
    for (auto& stmt : node.statements) {
        // Check if it is a component definition (Item)
        if (auto comp = std::dynamic_pointer_cast<ComponentNode>(stmt)) {
            if (comp->type == "Item" && !comp->name.empty()) {
                registry.registerComponent(comp->name, comp);
            }
            // Also visit children if needed (e.g., nested definitions? Nota doesn't seem to support nested defs explicitly yet)
        }
        stmt->accept(*this);
    }
}

void SemanticAnalyzer::visit(ImportNode& node) {
    // In future, this would load the module and register its exported components
}

void SemanticAnalyzer::visit(ComponentNode& node) {
    // No-op for now
}

void SemanticAnalyzer::visit(PropertyNode& node) {
    // No-op
}

void SemanticAnalyzer::visit(ValueNode& node) {
    // No-op
}
