#pragma once

#include "ast/AST.h"
#include <string>
#include <vector>

// Note: This class now implements both expression and statement visitors.
class ASTPrinter : public ExprVisitor<std::any>, public StmtVisitor<std::any> {
public:
    std::string print(const Expr& expr);
    std::string print(const Stmt& stmt);

    // Expression visitors
    std::any visit(const Binary& expr) override;
    std::any visit(const Grouping& expr) override;
    std::any visit(const Literal& expr) override;
    std::any visit(const Unary& expr) override;
    std::any visit(const Variable& expr) override;

    // Statement visitors
    std::any visit(const ExpressionStmt& stmt) override;
    std::any visit(const VarStmt& stmt) override;
    std::any visit(const IfStmt& stmt) override;


private:
    std::string parenthesize(const std::string& name, const std::vector<const Expr*>& exprs);
    std::string parenthesize(const std::string& name, const std::vector<const Expr*>& exprs, const std::vector<const Stmt*>& stmts);
};
