#ifndef NOTA_AST_PRINTER_H
#define NOTA_AST_PRINTER_H

#include "Stmt.h"
#include "Expr.h"
#include <string>

namespace nota {
namespace ast {

    class AstPrinter : public StmtVisitor<std::string>, public ExprVisitor<std::string> {
    public:
        std::string print(const std::vector<std::unique_ptr<Stmt>>& statements);

        std::string visit(class VarDeclStmt& stmt) override;
        std::string visit(class BlockStmt& stmt) override;
        std::string visit(class IfStmt& stmt) override;
        std::string visit(class WhileStmt& stmt) override;
        std::string visit(class DoWhileStmt& stmt) override;
        std::string visit(class ExpressionStmt& stmt) override;
        std::string visit(class ForEachStmt& stmt) override;
        std::string visit(class ForStmt& stmt) override;
        std::string visit(class MatchStmt& stmt) override;
        std::string visit(class FuncDeclStmt& stmt) override;
        std::string visit(class ReturnStmt& stmt) override;

        std::string visit(class LiteralExpr& expr) override;
        std::string visit(class UnaryExpr& expr) override;
        std::string visit(class BinaryExpr& expr) override;
        std::string visit(class VariableExpr& expr) override;
        std::string visit(class AssignExpr& expr) override;
        std::string visit(class PostfixExpr& expr) override;
        std::string visit(class CallExpr& expr) override;
    };

} // namespace ast
} // namespace nota

#endif // NOTA_AST_PRINTER_H
