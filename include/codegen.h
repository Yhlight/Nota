#pragma once

#include "ast.h"
#include "sema.h"
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>

class CodeGen : public ASTVisitor {
public:
    std::string generate(ProgramNode& root);

    void visit(ProgramNode& node) override;
    void visit(ImportNode& node) override;
    void visit(ComponentNode& node) override;
    void visit(PropertyNode& node) override;
    void visit(LiteralNode& node) override;
    void visit(ReferenceNode& node) override;
    void visit(BinaryExpressionNode& node) override;

private:
    std::stringstream html;
    std::stringstream css;
    int indentLevel = 0;
    ComponentRegistry registry;

    // Helper to capture expression output to a string
    std::string evaluateExpression(ASTNode& node);

    void indent(std::stringstream& ss);
    std::string mapComponentToTag(const std::string& type);
    std::string mapPropertyToCSS(const std::string& name);
    void generateStyleAttribute(const std::vector<std::shared_ptr<ASTNode>>& properties, const std::vector<std::shared_ptr<ASTNode>>& overrideProperties);
    void generateEvents(const std::vector<std::shared_ptr<ASTNode>>& properties, const std::vector<std::shared_ptr<ASTNode>>& overrideProperties);

    // Internal helper to pass overrides down the tree
    void visitComponent(ComponentNode& node, const std::unordered_map<std::string, std::vector<std::shared_ptr<ASTNode>>>& pendingOverrides);
};
