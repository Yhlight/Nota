#ifndef NOTA_AST_HPP
#define NOTA_AST_HPP

#include "lexer.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nota {

// Forward declarations
struct BinaryExpr;
struct GroupingExpr;
struct LiteralExpr;
struct UnaryExpr;
struct VariableExpr;
struct AssignExpr;
struct CallExpr;
struct LambdaExpr;

// Visitor for Expressions
struct ExprVisitor {
    virtual std::string visit(const BinaryExpr& expr) = 0;
    virtual std::string visit(const GroupingExpr& expr) = 0;
    virtual std::string visit(const LiteralExpr& expr) = 0;
    virtual std::string visit(const UnaryExpr& expr) = 0;
    virtual std::string visit(const VariableExpr& expr) = 0;
    virtual std::string visit(const AssignExpr& expr) = 0;
    virtual std::string visit(const CallExpr& expr) = 0;
    virtual std::string visit(const LambdaExpr& expr) = 0;
    virtual ~ExprVisitor() = default;
};

// Base class for all expression nodes
struct Expr {
    virtual ~Expr() = default;
    virtual std::string accept(ExprVisitor& visitor) const = 0;
};

// Forward declarations
struct ExpressionStmt;
struct VarStmt;
struct BlockStmt;
struct IfStmt;
struct WhileStmt;
struct ForStmt;
struct FunctionStmt;
struct ReturnStmt;


// Visitor for Statements
struct StmtVisitor {
    virtual std::string visit(const ExpressionStmt& stmt) = 0;
    virtual std::string visit(const VarStmt& stmt) = 0;
    virtual std::string visit(const BlockStmt& stmt) = 0;
    virtual std::string visit(const IfStmt& stmt) = 0;
    virtual std::string visit(const WhileStmt& stmt) = 0;
    virtual std::string visit(const ForStmt& stmt) = 0;
    virtual std::string visit(const FunctionStmt& stmt) = 0;
    virtual std::string visit(const ReturnStmt& stmt) = 0;
    virtual ~StmtVisitor() = default;
};


// Base class for all statement nodes
struct Stmt {
    virtual ~Stmt() = default;
    virtual std::string accept(StmtVisitor& visitor) const = 0;
};

// Concrete Expression Nodes
struct BinaryExpr : Expr {
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

    std::string accept(ExprVisitor& visitor) const override { return visitor.visit(*this); }
};

struct GroupingExpr : Expr {
    std::unique_ptr<Expr> expression;

    GroupingExpr(std::unique_ptr<Expr> expression) : expression(std::move(expression)) {}

    std::string accept(ExprVisitor& visitor) const override { return visitor.visit(*this); }
};

struct LiteralExpr : Expr {
    // Using std::string for simplicity; will refine later for different literal types.
    std::string value;

    LiteralExpr(std::string value) : value(std::move(value)) {}

    std::string accept(ExprVisitor& visitor) const override { return visitor.visit(*this); }
};

struct UnaryExpr : Expr {
    Token op;
    std::unique_ptr<Expr> right;

    UnaryExpr(Token op, std::unique_ptr<Expr> right)
        : op(op), right(std::move(right)) {}

    std::string accept(ExprVisitor& visitor) const override { return visitor.visit(*this); }
};

struct VariableExpr : Expr {
    Token name;

    VariableExpr(Token name) : name(name) {}

    std::string accept(ExprVisitor& visitor) const override { return visitor.visit(*this); }
};

struct AssignExpr : Expr {
    Token name;
    std::unique_ptr<Expr> value;

    AssignExpr(Token name, std::unique_ptr<Expr> value)
        : name(name), value(std::move(value)) {}

    std::string accept(ExprVisitor& visitor) const override { return visitor.visit(*this); }
};

struct CallExpr : Expr {
    std::unique_ptr<Expr> callee;
    Token paren; // To report location of '('.
    std::vector<std::unique_ptr<Expr>> arguments;

    CallExpr(std::unique_ptr<Expr> callee, Token paren, std::vector<std::unique_ptr<Expr>> arguments)
        : callee(std::move(callee)), paren(paren), arguments(std::move(arguments)) {}

    std::string accept(ExprVisitor& visitor) const override { return visitor.visit(*this); }
};

struct LambdaExpr : Expr {
    std::vector<Token> params;
    std::unique_ptr<Expr> body;

    LambdaExpr(std::vector<Token> params, std::unique_ptr<Expr> body)
        : params(std::move(params)), body(std::move(body)) {}

    std::string accept(ExprVisitor& visitor) const override { return visitor.visit(*this); }
};

// Concrete Statement Nodes
struct ExpressionStmt : Stmt {
    std::unique_ptr<Expr> expression;

    ExpressionStmt(std::unique_ptr<Expr> expression) : expression(std::move(expression)) {}

    std::string accept(StmtVisitor& visitor) const override { return visitor.visit(*this); }
};

struct VarStmt : Stmt {
    Token name;
    std::unique_ptr<Expr> initializer;
    bool is_mutable;

    VarStmt(Token name, std::unique_ptr<Expr> initializer, bool is_mutable)
        : name(name), initializer(std::move(initializer)), is_mutable(is_mutable) {}

    std::string accept(StmtVisitor& visitor) const override { return visitor.visit(*this); }
};

struct BlockStmt : Stmt {
    std::vector<std::unique_ptr<Stmt>> statements;

    BlockStmt(std::vector<std::unique_ptr<Stmt>> statements) : statements(std::move(statements)) {}

    std::string accept(StmtVisitor& visitor) const override { return visitor.visit(*this); }
};

struct IfStmt : Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> then_branch;
    std::unique_ptr<Stmt> else_branch;

    IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> then_branch, std::unique_ptr<Stmt> else_branch)
        : condition(std::move(condition)), then_branch(std::move(then_branch)), else_branch(std::move(else_branch)) {}

    std::string accept(StmtVisitor& visitor) const override { return visitor.visit(*this); }
};

struct WhileStmt : Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;

    WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
        : condition(std::move(condition)), body(std::move(body)) {}

    std::string accept(StmtVisitor& visitor) const override { return visitor.visit(*this); }
};

struct ForStmt : Stmt {
    std::unique_ptr<Stmt> initializer;
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Expr> increment;
    std::unique_ptr<Stmt> body;

    ForStmt(std::unique_ptr<Stmt> initializer, std::unique_ptr<Expr> condition, std::unique_ptr<Expr> increment, std::unique_ptr<Stmt> body)
        : initializer(std::move(initializer)), condition(std::move(condition)), increment(std::move(increment)), body(std::move(body)) {}

    std::string accept(StmtVisitor& visitor) const override { return visitor.visit(*this); }
};

struct FunctionStmt : Stmt {
    Token name;
    std::vector<Token> params;
    std::unique_ptr<Stmt> body;

    FunctionStmt(Token name, std::vector<Token> params, std::unique_ptr<Stmt> body)
        : name(name), params(std::move(params)), body(std::move(body)) {}

    std::string accept(StmtVisitor& visitor) const override { return visitor.visit(*this); }
};

struct ReturnStmt : Stmt {
    Token keyword;
    std::unique_ptr<Expr> value;

    ReturnStmt(Token keyword, std::unique_ptr<Expr> value)
        : keyword(keyword), value(std::move(value)) {}

    std::string accept(StmtVisitor& visitor) const override { return visitor.visit(*this); }
};


} // namespace nota

#endif // NOTA_AST_HPP
