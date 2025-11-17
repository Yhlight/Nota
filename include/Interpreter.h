#ifndef NOTA_INTERPRETER_H
#define NOTA_INTERPRETER_H

#include "AST/Expr.h"
#include "AST/Stmt.h"
#include "Environment.h"
#include "Value.h"
#include <any>
#include <memory>
#include <vector>

namespace nota {

class Interpreter : public ast::ExprVisitor, public ast::StmtVisitor {
  public:
    Interpreter();

    void interpret(const std::vector<std::unique_ptr<ast::Stmt>> &statements);

    std::any visit(ast::LiteralExpr &expr) override;
    std::any visit(ast::UnaryExpr &expr) override;
    std::any visit(ast::BinaryExpr &expr) override;
    std::any visit(ast::VariableExpr &expr) override;
    std::any visit(ast::AssignExpr &expr) override;
    std::any visit(ast::PostfixExpr &expr) override;
    std::any visit(ast::CallExpr &expr) override;
    std::any visit(ast::LambdaExpr &expr) override;
    std::any visit(ast::ArrayLiteralExpr &expr) override;
    std::any visit(ast::SubscriptExpr &expr) override;
    std::any visit(ast::GetExpr &expr) override;
    std::any visit(ast::SetExpr &expr) override;
    std::any visit(ast::ThisExpr &expr) override;

    std::any visit(ast::VarDeclStmt &stmt) override;
    std::any visit(ast::BlockStmt &stmt) override;
    std::any visit(ast::IfStmt &stmt) override;
    std::any visit(ast::WhileStmt &stmt) override;
    std::any visit(ast::DoWhileStmt &stmt) override;
    std::any visit(ast::ExpressionStmt &stmt) override;
    std::any visit(ast::ForEachStmt &stmt) override;
    std::any visit(ast::ForStmt &stmt) override;
    std::any visit(ast::MatchStmt &stmt) override;
    std::any visit(ast::FuncDeclStmt &stmt) override;
    std::any visit(ast::ReturnStmt &stmt) override;
    std::any visit(ast::ClassDeclStmt &stmt) override;

    void execute_block(const std::vector<std::unique_ptr<ast::Stmt>> &statements,
                       std::shared_ptr<Environment> environment);

  private:
    friend class NotaFunction;
    Value evaluate(ast::Expr &expr);
    void execute(ast::Stmt &stmt);

    bool is_truthy(const Value &value);

    std::shared_ptr<Environment> globals;
    std::shared_ptr<Environment> environment;
};

} // namespace nota

#endif // NOTA_INTERPRETER_H
