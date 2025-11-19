#pragma once

#include "Token.h"
#include <memory>
#include <vector>

// Forward declarations
struct Binary;
struct Grouping;
struct Literal;
struct Unary;
struct Variable;
struct Assign;
struct Postfix;
struct Call;

// Visitor interface
class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;
    virtual void visit(const Binary& expr) = 0;
    virtual void visit(const Grouping& expr) = 0;
    virtual void visit(const Literal& expr) = 0;
    virtual void visit(const Unary& expr) = 0;
    virtual void visit(const Variable& expr) = 0;
    virtual void visit(const Assign& expr) = 0;
    virtual void visit(const Postfix& expr) = 0;
    virtual void visit(const Call& expr) = 0;
};

// Base class for all expression nodes
class Expr {
public:
    virtual ~Expr() = default;
    virtual void accept(ExprVisitor& visitor) const = 0;
};

// Concrete expression classes
struct Binary : Expr {
    Binary(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const std::shared_ptr<Expr> left;
    const Token op;
    const std::shared_ptr<Expr> right;
};

struct Grouping : Expr {
    explicit Grouping(std::shared_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const std::shared_ptr<Expr> expression;
};

struct Literal : Expr {
    explicit Literal(std::variant<std::monostate, int, double, std::string, bool> value)
        : value(std::move(value)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const std::variant<std::monostate, int, double, std::string, bool> value;
};

struct Unary : Expr {
    Unary(Token op, std::shared_ptr<Expr> right)
        : op(std::move(op)), right(std::move(right)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const Token op;
    const std::shared_ptr<Expr> right;
};

struct Variable : Expr {
    explicit Variable(Token name)
        : name(std::move(name)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const Token name;
};

struct Assign : Expr {
    Assign(Token name, std::shared_ptr<Expr> value)
        : name(std::move(name)), value(std::move(value)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const Token name;
    const std::shared_ptr<Expr> value;
};

struct Postfix : Expr {
    Postfix(std::shared_ptr<Expr> left, Token op)
        : left(std::move(left)), op(std::move(op)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const std::shared_ptr<Expr> left;
    const Token op;
};

struct Call : Expr {
    Call(std::shared_ptr<Expr> callee, Token paren, std::vector<std::shared_ptr<Expr>> arguments)
        : callee(std::move(callee)), paren(std::move(paren)), arguments(std::move(arguments)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const std::shared_ptr<Expr> callee;
    const Token paren;
    const std::vector<std::shared_ptr<Expr>> arguments;
};
