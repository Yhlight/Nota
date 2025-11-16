#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "Expr.h"
#include "Stmt.h"
#include "Environment.h"
#include <vector>
#include <string>
#include <memory>

class Interpreter : public ExprVisitor<std::any>, public StmtVisitor<std::any> {
public:
    Interpreter();
    void interpret(const std::vector<std::unique_ptr<Stmt>>& statements);

    // Expression visitors
    std::any visitAssignExpr(const Assign& expr) override;
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
    std::any visitWhileStmt(const While& stmt) override;

private:
    std::shared_ptr<Environment> environment;

    std::any evaluate(const Expr& expr);
    void execute(const Stmt& stmt);
    void executeBlock(const std::vector<std::unique_ptr<Stmt>>& statements, std::shared_ptr<Environment> environment);


    bool isTruthy(const std::any& object);
    bool isEqual(const std::any& a, const std::any& b);
    void checkNumberOperand(const Token& op, const std::any& operand);
    void checkNumberOperands(const Token& op, const std::any& left, const std::any& right);

    std::string stringify(const std::any& object);

};

#endif // INTERPRETER_H
