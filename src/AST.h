#pragma once

#include "Token.h"
#include <memory>
#include <vector>
#include <any>

// Forward declarations for all AST node classes
struct Binary;
struct Grouping;
struct Literal;
struct Unary;
struct Variable;

// Visitor interface for expressions
template <typename R>
struct ExprVisitor {
    virtual R visit(const Binary& expr) = 0;
    virtual R visit(const Grouping& expr) = 0;
    virtual R visit(const Literal& expr) = 0;
    virtual R visit(const Unary& expr) = 0;
    virtual R visit(const Variable& expr) = 0;
};

// Base class for all expression nodes
struct Expr {
    virtual ~Expr() = default;
    virtual std::any accept(ExprVisitor<std::any>& visitor) const = 0;
};

// Concrete expression classes
struct Binary final : Expr {
    const std::unique_ptr<Expr> left;
    const Token op;
    const std::unique_ptr<Expr> right;

    Binary(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    std::any accept(ExprVisitor<std::any>& visitor) const override {
        return visitor.visit(*this);
    }
};

struct Grouping final : Expr {
    const std::unique_ptr<Expr> expression;

    explicit Grouping(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    std::any accept(ExprVisitor<std::any>& visitor) const override {
        return visitor.visit(*this);
    }
};

struct Literal final : Expr {
    const std::any value;

    explicit Literal(std::any value) : value(std::move(value)) {}

    std::any accept(ExprVisitor<std::any>& visitor) const override {
        return visitor.visit(*this);
    }
};

struct Unary final : Expr {
    const Token op;
    const std::unique_ptr<Expr> right;

    Unary(Token op, std::unique_ptr<Expr> right)
        : op(std::move(op)), right(std::move(right)) {}

    std::any accept(ExprVisitor<std::any>& visitor) const override {
        return visitor.visit(*this);
    }
};

struct Variable final : Expr {
    const Token name;

    explicit Variable(Token name) : name(std::move(name)) {}

    std::any accept(ExprVisitor<std::any>& visitor) const override {
        return visitor.visit(*this);
    }
};


// Forward declarations for all statement classes
struct ExpressionStmt;
struct PrintStmt;
struct VarStmt;

// Visitor interface for statements
template <typename R>
struct StmtVisitor {
    virtual R visit(const ExpressionStmt& stmt) = 0;
    virtual R visit(const PrintStmt& stmt) = 0;
    virtual R visit(const VarStmt& stmt) = 0;
};

// Base class for all statement nodes
struct Stmt {
    virtual ~Stmt() = default;
    virtual std::any accept(StmtVisitor<std::any>& visitor) const = 0;
};
