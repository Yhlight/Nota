#pragma once

#include "ast.h"
#include <unordered_map>
#include <string>
#include <memory>
#include <set>

class ComponentRegistry {
public:
    void registerComponent(const std::string& name, std::shared_ptr<ComponentNode> definition);
    std::shared_ptr<ComponentNode> getComponent(const std::string& name) const;
    bool hasComponent(const std::string& name) const;

private:
    std::unordered_map<std::string, std::shared_ptr<ComponentNode>> components;
};

class SemanticAnalyzer : public ASTVisitor {
public:
    SemanticAnalyzer(ComponentRegistry& registry) : registry(registry) {}

    void analyze(ProgramNode& root);
    // For recursion with cycle detection
    void analyze(ProgramNode& root, std::set<std::string>& visitedPaths);

    void visit(ProgramNode& node) override;
    void visit(ImportNode& node) override;
    void visit(PackageNode& node) override;
    void visit(ComponentNode& node) override;
    void visit(PropertyNode& node) override;
    // Updated visitor methods for new AST
    void visit(LiteralNode& node) override;
    void visit(ReferenceNode& node) override;
    void visit(BinaryExpressionNode& node) override;
    void visit(ConditionalNode& node) override;
    void visit(StructDefinitionNode& node) override;
    void visit(StructInstantiationNode& node) override;
    void visit(ListNode& node) override;
    void visit(DelegateNode& node) override;
    void visit(ForNode& node) override;

private:
    ComponentRegistry& registry;
    std::set<std::string> visitedPaths;
};
