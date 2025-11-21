#pragma once

#include "AST.h"
#include "Environment.h"
#include <memory>

namespace nota {

class Interpreter : public StmtVisitor, public ExprVisitor {
public:
    void interpret(const std::vector<std::shared_ptr<Stmt>>& statements);
    Environment& getEnvironment() { return environment_; }

    void visit(const std::shared_ptr<Block>& stmt) override;
    void visit(const std::shared_ptr<ExpressionStmt>& stmt) override;
    void visit(const std::shared_ptr<VarStmt>& stmt) override;
    void visit(const std::shared_ptr<IfStmt>& stmt) override;
    void visit(const std::shared_ptr<WhileStmt>& stmt) override;
    void visit(const std::shared_ptr<ForStmt>& stmt) override;

    void visit(const std::shared_ptr<Binary>& expr) override;
    void visit(const std::shared_ptr<Grouping>& expr) override;
    void visit(const std::shared_ptr<Literal>& expr) override;
    void visit(const std::shared_ptr<Unary>& expr) override;
    void visit(const std::shared_ptr<Variable>& expr) override;
    void visit(const std::shared_ptr<Assign>& expr) override;
    void visit(const std::shared_ptr<Postfix>& expr) override;

private:
    using Value = std::variant<std::monostate, int, double, std::string, bool>;

    void execute(const std::shared_ptr<Stmt>& stmt);
    Value evaluate(const std::shared_ptr<Expr>& expr);
    bool isTruthy(const Value& value);

    Value lastValue_;
    Environment environment_;
};

} // namespace nota
