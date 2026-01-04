#pragma once

#include "../Token.h"
#include <any>
#include <memory>
#include <vector>

// Forward declarations
struct LiteralExpr;
struct IdentifierExpr;

// Visitor Interface
class ExprVisitor {
public:
    virtual std::any visit(const LiteralExpr& expr) = 0;
    virtual std::any visit(const IdentifierExpr& expr) = 0;
    virtual ~ExprVisitor() = default;
};

// Base class
class Expr {
public:
    virtual std::any accept(ExprVisitor& visitor) const = 0;
    virtual ~Expr() = default;
};

// Concrete classes
struct LiteralExpr : Expr {
    const std::any value;

    LiteralExpr(std::any value) : value(std::move(value)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visit(*this);
    }
};

struct IdentifierExpr : Expr {
    const Token name;

    IdentifierExpr(Token name) : name(std::move(name)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visit(*this);
    }
};
