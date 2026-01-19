#pragma once
#include "ast.h"
#include "sema.h"
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <set>

class CodeGen : public ASTVisitor {
public:
    std::stringstream html;
    std::stringstream css;
    std::stringstream js; // For Runtime JS
    int indentLevel = 0;
    ComponentRegistry registry;
    std::set<std::string> activeIterators; // Track loop iterators for interpolation

    std::string generate(ProgramNode& root);

    void visit(ProgramNode& node) override;
    void visit(ImportNode& node) override;
    void visit(PackageNode& node) override;
    void visit(ComponentNode& node) override;
    void visit(PropertyNode& node) override;
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
    void indent(std::stringstream& ss);
    std::string mapComponentToTag(const std::string& type);
    std::string mapPropertyToCSS(const std::string& name, bool isTextComponent);
    void generateStyleAttribute(const std::vector<std::shared_ptr<ASTNode>>& properties, const std::vector<std::shared_ptr<ASTNode>>& overrideProperties, bool isTextComponent);
    void generateEvents(const std::vector<std::shared_ptr<ASTNode>>& properties, const std::vector<std::shared_ptr<ASTNode>>& overrideProperties);
    std::string evaluateExpression(ASTNode& node);

    // For deep property overrides
    void visitComponent(ComponentNode& node, const std::unordered_map<std::string, std::vector<std::shared_ptr<ASTNode>>>& pendingOverrides);
    void visitComponentWithDirectOverrides(ComponentNode& node, const std::unordered_map<std::string, std::vector<std::shared_ptr<ASTNode>>>& pendingOverrides, const std::vector<std::shared_ptr<ASTNode>>& directOverrides);
};
