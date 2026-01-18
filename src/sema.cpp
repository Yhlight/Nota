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
    // Load module
    // path is string literal.
    // e.g. "ui.nota"
    // Remove quotes if present? Parser provides value token which usually has quotes stripped if we did it right.
    // Lexer::readString: strips quotes?
    // Let's check Lexer.
    // Lexer::readString: "if (peek() == '"') advance(); // consume closing " "
    // But it accumulates characters inside.
    // It seems it does NOT include the surrounding quotes in the token value unless I misread.
    // Let's verify Lexer::readString in src/lexer.cpp via read_file.

    // Assuming filename is clean.
    try {
        auto importedRoot = Compiler::parseFile(node.path);

        // Register exported components from imported module
        // For simplicity, we register ALL top-level Item definitions found in the imported file.
        // We handle aliasing.

        std::string prefix = "";
        if (!node.alias.empty()) {
            prefix = node.alias + ".";
        }

        for (auto& stmt : importedRoot->statements) {
             if (auto comp = std::dynamic_pointer_cast<ComponentNode>(stmt)) {
                if (comp->type == "Item" && !comp->name.empty()) {
                    // Register with prefix
                    registry.registerComponent(prefix + comp->name, comp);
                }
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "Warning: Failed to import " << node.path << ": " << e.what() << "\n";
    }
}

void SemanticAnalyzer::visit(ComponentNode& node) {
    // No-op for now
}

void SemanticAnalyzer::visit(PropertyNode& node) {
    // No-op
}

void SemanticAnalyzer::visit(LiteralNode& node) {
    // No-op
}

void SemanticAnalyzer::visit(ReferenceNode& node) {
    // No-op
}

void SemanticAnalyzer::visit(BinaryExpressionNode& node) {
    // No-op
}
