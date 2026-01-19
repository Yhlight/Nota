#pragma once
#include "Token.h"
#include <memory>
#include <string>
#include <vector>

// Forward decl
class ExprVisitor;

class Expr {
public:
    virtual ~Expr() = default;
    virtual std::string accept(ExprVisitor& visitor) = 0;
    virtual std::unique_ptr<Expr> clone() const = 0;
};

class BinaryExpr : public Expr {
public:
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

    std::string accept(ExprVisitor& visitor) override;

    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<BinaryExpr>(left->clone(), op, right->clone());
    }
};

class LiteralExpr : public Expr {
public:
    std::string value;
    TokenType type;

    LiteralExpr(std::string value, TokenType type) : value(value), type(type) {}

    std::string accept(ExprVisitor& visitor) override;

    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<LiteralExpr>(value, type);
    }
};

class VariableExpr : public Expr {
public:
    Token name;

    VariableExpr(Token name) : name(name) {}

    std::string accept(ExprVisitor& visitor) override;

    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<VariableExpr>(name);
    }
};

class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;
    virtual std::string visitBinaryExpr(BinaryExpr& expr) = 0;
    virtual std::string visitLiteralExpr(LiteralExpr& expr) = 0;
    virtual std::string visitVariableExpr(VariableExpr& expr) = 0;
};

// Implement accept methods inline
inline std::string BinaryExpr::accept(ExprVisitor& visitor) { return visitor.visitBinaryExpr(*this); }
inline std::string LiteralExpr::accept(ExprVisitor& visitor) { return visitor.visitLiteralExpr(*this); }
inline std::string VariableExpr::accept(ExprVisitor& visitor) { return visitor.visitVariableExpr(*this); }
