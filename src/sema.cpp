#include "sema.h"
#include "compiler.h"
#include "utils.h"
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
        std::string resolvedPath = Utils::resolveImportPath(node.path);
        auto importedRoot = Compiler::parseFile(resolvedPath);

        std::string prefix = "";
        if (!node.alias.empty()) {
            prefix = node.alias + ".";
        } else {
            // Check for Package declaration
            for (auto& stmt : importedRoot->statements) {
                if (auto pkg = std::dynamic_pointer_cast<PackageNode>(stmt)) {
                    prefix = pkg->name + ".";
                    break;
                }
            }

            // Fallback to filename if no package (and no alias)
            // if (prefix.empty()) {
            //    std::string filename = resolvedPath.substr(resolvedPath.find_last_of("/\\") + 1);
            //    size_t lastDot = filename.find_last_of('.');
            //    if (lastDot != std::string::npos) filename = filename.substr(0, lastDot);
            //    prefix = filename + ".";
            // }
            // Nota.md says file import -> filename is namespace.
            // But if package is present, package is namespace.
            // The logic above implements package priority.
        }

        for (auto& stmt : importedRoot->statements) {
             if (auto comp = std::dynamic_pointer_cast<ComponentNode>(stmt)) {
                if (comp->type == "Item" && !comp->name.empty() && comp->isExported) {
                    registry.registerComponent(prefix + comp->name, comp);
                }
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "Warning: Failed to import " << node.path << ": " << e.what() << "\n";
    }
}

void SemanticAnalyzer::visit(PackageNode& node) {
    // Current behavior: Do nothing.
    // In a full implementation, this would enforce namespace rules.
    // For prototype, we assume Imports handle naming.
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

void SemanticAnalyzer::visit(ListNode& node) {
    for (auto& element : node.elements) {
        element->accept(*this);
    }
}

void SemanticAnalyzer::visit(DelegateNode& node) {
}

void SemanticAnalyzer::visit(ForNode& node) {
    for (auto& child : node.body) {
        child->accept(*this);
    }
}
