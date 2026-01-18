#pragma once

#include "ast.h"
#include <string>
#include <iostream>
#include <sstream>

class CodeGen : public ASTVisitor {
public:
    std::string generate(ProgramNode& root);

    void visit(ProgramNode& node) override;
    void visit(ImportNode& node) override;
    void visit(ComponentNode& node) override;
    void visit(PropertyNode& node) override;
    void visit(ValueNode& node) override;

private:
    std::stringstream html;
    std::stringstream css;
    int indentLevel = 0;

    void indent(std::stringstream& ss);
    std::string mapComponentToTag(const std::string& type);
    std::string mapPropertyToCSS(const std::string& name);
};
