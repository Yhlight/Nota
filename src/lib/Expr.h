#ifndef NOTA_EXPR_H
#define NOTA_EXPR_H

#include "Token.h"
#include <vector>
#include <any>
#include <memory>

namespace nota {

struct Binary;
struct Grouping;
struct Literal;
struct Unary;
class Expr;

class ExprVisitor {
public:
    virtual std::any visitBinaryExpr(const Binary& expr) = 0;
    virtual std::any visitGroupingExpr(const Grouping& expr) = 0;
    virtual std::any visitLiteralExpr(const Literal& expr) = 0;
    virtual std::any visitUnaryExpr(const Unary& expr) = 0;
};

class Expr {
public:
    virtual ~Expr() = default;
    virtual std::any accept(ExprVisitor& visitor) const = 0;
};

struct Binary : public Expr {
    Binary(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visitBinaryExpr(*this);
    }

    std::unique_ptr<Expr> const left;
    Token const op;
    std::unique_ptr<Expr> const right;
};

struct Grouping : public Expr {
    Grouping(std::unique_ptr<Expr> expression) : expression(std::move(expression)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visitGroupingExpr(*this);
    }

    std::unique_ptr<Expr> const expression;
};

struct Literal : public Expr {
    Literal(std::any value, int line) : value(std::move(value)), line(line) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visitLiteralExpr(*this);
    }

    std::any const value;
    int const line;
};

struct Unary : public Expr {
    Unary(Token op, std::unique_ptr<Expr> right) : op(op), right(std::move(right)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visitUnaryExpr(*this);
    }

    Token const op;
    std::unique_ptr<Expr> const right;
};

} // namespace nota

#endif // NOTA_EXPR_H
