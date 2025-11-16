#ifndef EXPR_H
#define EXPR_H

#include "Token.h"
#include <memory>
#include <vector>
#include <any>

class Assign;
class Binary;
class Grouping;
class Literal;
class Unary;
class Variable;

template <typename R>
class ExprVisitor {
public:
    virtual R visitAssignExpr(const Assign& expr) = 0;
    virtual R visitBinaryExpr(const Binary& expr) = 0;
    virtual R visitGroupingExpr(const Grouping& expr) = 0;
    virtual R visitLiteralExpr(const Literal& expr) = 0;
    virtual R visitUnaryExpr(const Unary& expr) = 0;
    virtual R visitVariableExpr(const Variable& expr) = 0;
    virtual ~ExprVisitor() = default;
};

class Expr {
public:
    virtual ~Expr() = default;
    virtual std::any accept(ExprVisitor<std::any>& visitor) const = 0;
};

class Assign : public Expr {
public:
    Assign(Token name, std::unique_ptr<Expr> value) :
        name(std::move(name)),
        value(std::move(value)) {}

    std::any accept(ExprVisitor<std::any>& visitor) const override {
        return visitor.visitAssignExpr(*this);
    }

    const Token name;
    const std::unique_ptr<Expr> value;
};

class Binary : public Expr {
public:
    Binary(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right) :
        left(std::move(left)),
        op(std::move(op)),
        right(std::move(right)) {}

    std::any accept(ExprVisitor<std::any>& visitor) const override {
        return visitor.visitBinaryExpr(*this);
    }

    const std::unique_ptr<Expr> left;
    const Token op;
    const std::unique_ptr<Expr> right;
};

class Grouping : public Expr {
public:
    Grouping(std::unique_ptr<Expr> expression) :
        expression(std::move(expression)) {}

    std::any accept(ExprVisitor<std::any>& visitor) const override {
        return visitor.visitGroupingExpr(*this);
    }

    const std::unique_ptr<Expr> expression;
};

class Literal : public Expr {
public:
    Literal(std::any value) :
        value(std::move(value)) {}

    std::any accept(ExprVisitor<std::any>& visitor) const override {
        return visitor.visitLiteralExpr(*this);
    }

    const std::any value;
};

class Unary : public Expr {
public:
    Unary(Token op, std::unique_ptr<Expr> right) :
        op(std::move(op)),
        right(std::move(right)) {}

    std::any accept(ExprVisitor<std::any>& visitor) const override {
        return visitor.visitUnaryExpr(*this);
    }

    const Token op;
    const std::unique_ptr<Expr> right;
};

class Variable : public Expr {
public:
    Variable(Token name) :
        name(std::move(name)) {}

    std::any accept(ExprVisitor<std::any>& visitor) const override {
        return visitor.visitVariableExpr(*this);
    }

    const Token name;
};

#endif // EXPR_H
