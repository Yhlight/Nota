#pragma once

#include "Expr.h"
#include "Stmt.h"
#include "Token.h"
#include "Environment.h"
#include "NotaFunction.h" // Include the new header
#include <vector>
#include <memory>
#include <any>
#include <stdexcept>

class Interpreter : public ExprVisitor, public StmtVisitor {
public:
    Interpreter();
    void interpret(const std::vector<std::shared_ptr<Stmt>>& statements);
    std::any getLastValue() const;
    void executeBlock(const std::vector<std::shared_ptr<Stmt>>& statements, std::shared_ptr<Environment> environment);

    // Custom exception for return values
    class Return : public std::runtime_error {
    public:
        Return(std::any value) : std::runtime_error(""), value(std::move(value)) {}
        std::any value;
    };

    // Expression visitor methods
    void visit(const Binary& expr) override;
    void visit(const Grouping& expr) override;
    void visit(const Literal& expr) override;
    void visit(const Unary& expr) override;
    void visit(const Variable& expr) override;
    void visit(const Assign& expr) override;
    void visit(const Postfix& expr) override;
    void visit(const Call& expr) override;

    // Statement visitor methods
    void visit(const ExpressionStmt& stmt) override;
    void visit(const VarStmt& stmt) override;
    void visit(const BlockStmt& stmt) override;
    void visit(const IfStmt& stmt) override;
    void visit(const WhileStmt& stmt) override;
    void visit(const ForStmt& stmt) override;
    void visit(const DoWhileStmt& stmt) override;
    void visit(const FunctionStmt& stmt) override;
    void visit(const ReturnStmt& stmt) override;

private:
    friend class NotaFunction;
    std::shared_ptr<Environment> globals;
    std::shared_ptr<Environment> environment;
    std::any last_value;

    void execute(const std::shared_ptr<Stmt>& stmt);
    void evaluate(const std::shared_ptr<Expr>& expr);

    bool isTruthy(const std::any& value);
    bool isEqual(const std::any& a, const std::any& b);
};
