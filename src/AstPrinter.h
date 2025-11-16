#ifndef AST_PRINTER_H
#define AST_PRINTER_H

#include "Expr.h"
#include "Stmt.h"
#include <string>
#include <vector>

class AstPrinter : public ExprVisitor<std::any>, public StmtVisitor<std::any> {
public:
    std::string print(const Expr& expr);
    std::string print(const Stmt& stmt);

    // Expression visitors
    std::any visitBinaryExpr(const Binary& expr) override;
    std::any visitGroupingExpr(const Grouping& expr) override;
    std::any visitLiteralExpr(const Literal& expr) override;
    std::any visitUnaryExpr(const Unary& expr) override;
    std::any visitVariableExpr(const Variable& expr) override;

    // Statement visitors
    std::any visitBlockStmt(const Block& stmt) override;
    std::any visitExpressionStmt(const Expression& stmt) override;
    std::any visitIfStmt(const If& stmt) override;
    std::any visitPrintStmt(const Print& stmt) override;
    std::any visitVarStmt(const Var& stmt) override;


private:
    std::string parenthesize(const std::string& name, const std::vector<const Expr*>& exprs);
    std::string parenthesize(const std::string& name, const std::vector<const Stmt*>& stmts);
    std::string parenthesize(const std::string& name, const std::vector<const Expr*>& exprs, const std::vector<const Stmt*>& stmts);

};

#endif // AST_PRINTER_H
