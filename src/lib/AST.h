#pragma once

#include "Token.h"
#include <memory>
#include <vector>

namespace nota {

// Forward declarations
struct Binary;
struct Grouping;
struct Literal;
struct Unary;
struct Variable;
struct Assign;
struct Postfix;

class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;
    virtual void visit(const std::shared_ptr<Binary>& expr) = 0;
    virtual void visit(const std::shared_ptr<Grouping>& expr) = 0;
    virtual void visit(const std::shared_ptr<Literal>& expr) = 0;
    virtual void visit(const std::shared_ptr<Unary>& expr) = 0;
    virtual void visit(const std::shared_ptr<Variable>& expr) = 0;
    virtual void visit(const std::shared_ptr<Assign>& expr) = 0;
    virtual void visit(const std::shared_ptr<Postfix>& expr) = 0;
};

class Expr {
public:
    virtual ~Expr() = default;
    virtual void accept(ExprVisitor& visitor) = 0;
};

struct Binary : Expr, public std::enable_shared_from_this<Binary> {
    Binary(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
        : left(left), op(op), right(right) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    std::shared_ptr<Expr> left;
    Token op;
    std::shared_ptr<Expr> right;
};

struct Grouping : Expr, public std::enable_shared_from_this<Grouping> {
    Grouping(std::shared_ptr<Expr> expression)
        : expression(expression) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    std::shared_ptr<Expr> expression;
};

struct Literal : Expr, public std::enable_shared_from_this<Literal> {
    Literal(std::variant<std::monostate, int, double, std::string, bool> value)
        : value(value) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    std::variant<std::monostate, int, double, std::string, bool> value;
};

struct Unary : Expr, public std::enable_shared_from_this<Unary> {
    Unary(Token op, std::shared_ptr<Expr> right)
        : op(op), right(right) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    Token op;
    std::shared_ptr<Expr> right;
};

struct Variable : Expr, public std::enable_shared_from_this<Variable> {
    Variable(Token name)
        : name(name) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    Token name;
};

struct Assign : Expr, public std::enable_shared_from_this<Assign> {
    Assign(Token name, std::shared_ptr<Expr> value)
        : name(name), value(value) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    Token name;
    std::shared_ptr<Expr> value;
};

struct Postfix : Expr, public std::enable_shared_from_this<Postfix> {
    Postfix(std::shared_ptr<Expr> left, Token op)
        : left(left), op(op) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    std::shared_ptr<Expr> left;
    Token op;
};

// Statements
struct Block;
struct ExpressionStmt;
struct VarStmt;
struct IfStmt;
struct WhileStmt;
struct ForStmt;

class StmtVisitor {
public:
    virtual ~StmtVisitor() = default;
    virtual void visit(const std::shared_ptr<Block>& stmt) = 0;
    virtual void visit(const std::shared_ptr<ExpressionStmt>& stmt) = 0;
    virtual void visit(const std::shared_ptr<VarStmt>& stmt) = 0;
    virtual void visit(const std::shared_ptr<IfStmt>& stmt) = 0;
    virtual void visit(const std::shared_ptr<WhileStmt>& stmt) = 0;
    virtual void visit(const std::shared_ptr<ForStmt>& stmt) = 0;
};

class Stmt {
public:
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor& visitor) = 0;
};

struct Block : Stmt, public std::enable_shared_from_this<Block> {
    Block(std::vector<std::shared_ptr<Stmt>> statements)
        : statements(statements) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    std::vector<std::shared_ptr<Stmt>> statements;
};

struct ExpressionStmt : Stmt, public std::enable_shared_from_this<ExpressionStmt> {
    ExpressionStmt(std::shared_ptr<Expr> expression)
        : expression(expression) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    std::shared_ptr<Expr> expression;
};

struct VarStmt : Stmt, public std::enable_shared_from_this<VarStmt> {
    VarStmt(Token name, std::shared_ptr<Expr> initializer)
        : name(name), initializer(initializer) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    Token name;
    std::shared_ptr<Expr> initializer;
};

struct IfStmt : Stmt, public std::enable_shared_from_this<IfStmt> {
    IfStmt(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> thenBranch, std::shared_ptr<Stmt> elseBranch)
        : condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> thenBranch;
    std::shared_ptr<Stmt> elseBranch;
};

struct WhileStmt : Stmt, public std::enable_shared_from_this<WhileStmt> {
    WhileStmt(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body)
        : condition(condition), body(body) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> body;
};

struct ForStmt : Stmt, public std::enable_shared_from_this<ForStmt> {
    ForStmt(std::shared_ptr<Stmt> initializer, std::shared_ptr<Expr> condition,
            std::shared_ptr<Expr> increment, std::shared_ptr<Stmt> body)
        : initializer(initializer), condition(condition), increment(increment), body(body) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    std::shared_ptr<Stmt> initializer;
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Expr> increment;
    std::shared_ptr<Stmt> body;
};


} // namespace nota
