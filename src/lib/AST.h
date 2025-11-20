#pragma once

#include "Token.h"
#include <memory>

namespace nota {

// Forward declarations
struct Binary;
struct Grouping;
struct Literal;
struct Unary;

class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;
    virtual void visit(const std::shared_ptr<Binary>& expr) = 0;
    virtual void visit(const std::shared_ptr<Grouping>& expr) = 0;
    virtual void visit(const std::shared_ptr<Literal>& expr) = 0;
    virtual void visit(const std::shared_ptr<Unary>& expr) = 0;
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
    Literal(std::variant<std::monostate, int, double, std::string> value)
        : value(value) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    std::variant<std::monostate, int, double, std::string> value;
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

} // namespace nota
