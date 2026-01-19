#include "sema.h"
#include "compiler.h"
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
        if (auto comp = std::dynamic_pointer_cast<ComponentNode>(stmt)) {
            if (comp->type == "Item" && !comp->name.empty()) {
                registry.registerComponent(comp->name, comp);
            }
        }
        stmt->accept(*this);
    }
}

void SemanticAnalyzer::visit(ImportNode& node) {
    try {
        auto importedRoot = Compiler::parseFile(node.path);

        std::string prefix = "";
        if (!node.alias.empty()) {
            prefix = node.alias + ".";
        }

        for (auto& stmt : importedRoot->statements) {
             if (auto comp = std::dynamic_pointer_cast<ComponentNode>(stmt)) {
                if (comp->type == "Item" && !comp->name.empty()) {
                    registry.registerComponent(prefix + comp->name, comp);
                }
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "Warning: Failed to import " << node.path << ": " << e.what() << "\n";
    }
}

void SemanticAnalyzer::visit(ComponentNode& node) {
}

void SemanticAnalyzer::visit(PropertyNode& node) {
}

void SemanticAnalyzer::visit(LiteralNode& node) {
}

void SemanticAnalyzer::visit(ReferenceNode& node) {
}

void SemanticAnalyzer::visit(BinaryExpressionNode& node) {
}

void SemanticAnalyzer::visit(ConditionalNode& node) {
}

void SemanticAnalyzer::visit(StructDefinitionNode& node) {
}

void SemanticAnalyzer::visit(StructInstantiationNode& node) {
    for (auto& arg : node.arguments) {
        arg->accept(*this);
    }
}
