#pragma once

#include "AST/Expr.h"
#include "AST/Stmt.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>

struct CompilationResult {
    std::string html;
    std::string css;
};

class CodeGenerator : public ExprVisitor, public StmtVisitor {
public:
    CompilationResult generate(const std::vector<std::unique_ptr<Stmt>>& statements);

private:
    std::any visit(const ComponentStmt& stmt) override;
    std::any visit(const PropertyStmt& stmt) override;

    std::any visit(const LiteralExpr& expr) override;
    std::any visit(const IdentifierExpr& expr) override;
    std::any visit(const BinaryExpr& expr) override;
    std::any visit(const GetExpr& expr) override;

    std::stringstream html_out;
    std::stringstream css_out;
    int class_counter = 0;
};
