#pragma once

#include "AST.h"
#include "Environment.h"
#include <memory>
#include <stdexcept>
#include <vector>
#include <map>

namespace nota {

class ModuleLoader; // Forward declaration

class Interpreter : public StmtVisitor, public ExprVisitor {
public:
    class RuntimeError : public std::runtime_error {
    public:
        RuntimeError(const Token& token, const std::string& message)
            : std::runtime_error(message), token(token) {}
        const Token token;
    };

    class ReturnControl : public std::exception {
    public:
        ReturnControl(Value value) : value(value) {}
        const Value value;
    };

    Interpreter();
    ~Interpreter(); // Add destructor declaration
    void interpret(const std::vector<std::shared_ptr<Stmt>>& statements);
    void executeBlock(const std::vector<std::shared_ptr<Stmt>>& statements, std::shared_ptr<Environment> environment);
    std::shared_ptr<Environment> getEnvironment() { return environment_; }

    void visit(const std::shared_ptr<Block>& stmt) override;
    void visit(const std::shared_ptr<ExpressionStmt>& stmt) override;
    void visit(const std::shared_ptr<VarStmt>& stmt) override;
    void visit(const std::shared_ptr<IfStmt>& stmt) override;
    void visit(const std::shared_ptr<WhileStmt>& stmt) override;
    void visit(const std::shared_ptr<DoWhileStmt>& stmt) override;
    void visit(const std::shared_ptr<FunctionStmt>& stmt) override;
    void visit(const std::shared_ptr<ReturnStmt>& stmt) override;
    void visit(const std::shared_ptr<ClassStmt>& stmt) override;
    void visit(const std::shared_ptr<ImportStmt>& stmt) override;
    void visit(const std::shared_ptr<PackageStmt>& stmt) override;

    void visit(const std::shared_ptr<Binary>& expr) override;
    void visit(const std::shared_ptr<Grouping>& expr) override;
    void visit(const std::shared_ptr<Literal>& expr) override;
    void visit(const std::shared_ptr<Unary>& expr) override;
    void visit(const std::shared_ptr<Variable>& expr) override;
    void visit(const std::shared_ptr<Assign>& expr) override;
    void visit(const std::shared_ptr<Postfix>& expr) override;
    void visit(const std::shared_ptr<CallExpr>& expr) override;
    void visit(const std::shared_ptr<GetExpr>& expr) override;
    void visit(const std::shared_ptr<SetExpr>& expr) override;
    void visit(const std::shared_ptr<ThisExpr>& expr) override;
    void visit(const std::shared_ptr<ModuleAccessExpr>& expr) override;

private:
    void execute(const std::shared_ptr<Stmt>& stmt);
    Value evaluate(const std::shared_ptr<Expr>& expr);
    bool isTruthy(const Value& value);

    Value lastValue_;
    std::shared_ptr<Environment> environment_;
    std::unique_ptr<ModuleLoader> moduleLoader_;
    std::map<std::string, std::shared_ptr<Environment>> modules_;
};

} // namespace nota
