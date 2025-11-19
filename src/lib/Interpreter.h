#pragma once

#include "Expr.h"
#include "Stmt.h"
#include "Token.h"
#include "Environment.h"
#include <vector>
#include <memory>

class Interpreter : public ExprVisitor, public StmtVisitor {
public:
    Interpreter();
    void interpret(const std::vector<std::shared_ptr<Stmt>>& statements);
    std::shared_ptr<Environment> getGlobals();

    // Expression visitor methods
    void visit(const Binary& expr) override;
    void visit(const Grouping& expr) override;
    void visit(const Literal& expr) override;
    void visit(const Unary& expr) override;
    void visit(const Variable& expr) override;
    void visit(const Assign& expr) override;
    void visit(const Postfix& expr) override;

    // Statement visitor methods
    void visit(const ExpressionStmt& stmt) override;
    void visit(const VarStmt& stmt) override;
    void visit(const BlockStmt& stmt) override;
    void visit(const IfStmt& stmt) override;
    void visit(const WhileStmt& stmt) override;
    void visit(const ForStmt& stmt) override;
    void visit(const DoWhileStmt& stmt) override;

private:
    std::shared_ptr<Environment> globals;
    std::shared_ptr<Environment> environment;
    std::variant<std::monostate, int, double, std::string, bool> last_value;

    void execute(const std::shared_ptr<Stmt>& stmt);
    void executeBlock(const std::vector<std::shared_ptr<Stmt>>& statements, std::shared_ptr<Environment> environment);
    void evaluate(const std::shared_ptr<Expr>& expr);

    bool isTruthy(const std::variant<std::monostate, int, double, std::string, bool>& value);
    bool isEqual(const std::variant<std::monostate, int, double, std::string, bool>& a, const std::variant<std::monostate, int, double, std::string, bool>& b);
};
