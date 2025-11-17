#ifndef NOTA_AST_PRINTER_H
#define NOTA_AST_PRINTER_H

#include "Stmt.h"
#include "Expr.h"
#include "Type.h"
#include <string>

namespace nota {
namespace ast {

    class AstPrinter : public StmtVisitor, public ExprVisitor, public TypeVisitor {
    public:
        std::string print(const std::vector<std::unique_ptr<Stmt>>& statements);

        std::any visit(class VarDeclStmt& stmt) override;
        std::any visit(class BlockStmt& stmt) override;
        std::any visit(class IfStmt& stmt) override;
        std::any visit(class WhileStmt& stmt) override;
        std::any visit(class DoWhileStmt& stmt) override;
        std::any visit(class ExpressionStmt& stmt) override;
        std::any visit(class ForEachStmt& stmt) override;
        std::any visit(class ForStmt& stmt) override;
        std::any visit(class MatchStmt& stmt) override;
        std::any visit(class FuncDeclStmt& stmt) override;
        std::any visit(class ReturnStmt& stmt) override;
        std::any visit(class ClassDeclStmt& stmt) override;

        std::any visit(class LiteralExpr& expr) override;
        std::any visit(class UnaryExpr& expr) override;
        std::any visit(class BinaryExpr& expr) override;
        std::any visit(class VariableExpr& expr) override;
        std::any visit(class AssignExpr& expr) override;
        std::any visit(class PostfixExpr& expr) override;
        std::any visit(class CallExpr& expr) override;
        std::any visit(class LambdaExpr& expr) override;
        std::any visit(class ArrayLiteralExpr& expr) override;
        std::any visit(class SubscriptExpr& expr) override;
        std::any visit(class GetExpr& expr) override;
        std::any visit(class SetExpr& expr) override;
        std::any visit(class ThisExpr& expr) override;

        std::any visit(class BaseType& type) override;
        std::any visit(class ArrayType& type) override;
    };

} // namespace ast
} // namespace nota

#endif // NOTA_AST_PRINTER_H
