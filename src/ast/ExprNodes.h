#pragma once

#include "Expr.h"
#include "Token.h"
#include "Visitor.h"
#include <memory>

class BinaryExpr : public Expr {
public:
    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

    std::any accept(Visitor& visitor) override {
        return visitor.visit(*this);
    }

    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;
};

class UnaryExpr : public Expr {
public:
    UnaryExpr(Token op, std::unique_ptr<Expr> right)
        : op(op), right(std::move(right)) {}

    std::any accept(Visitor& visitor) override {
        return visitor.visit(*this);
    }

    Token op;
    std::unique_ptr<Expr> right;
};

class LiteralExpr : public Expr {
public:
    LiteralExpr(Token value) : value(value) {}

    std::any accept(Visitor& visitor) override {
        return visitor.visit(*this);
    }

    Token value;
};

class VariableExpr : public Expr {
public:
    VariableExpr(Token name) : name(name) {}

    std::any accept(Visitor& visitor) override {
        return visitor.visit(*this);
    }

    Token name;
};
