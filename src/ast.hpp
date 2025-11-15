#pragma once

#include "token.hpp"
#include <memory>
#include <variant>
#include <vector>

struct BinaryExpr;
struct GroupingExpr;
struct LiteralExpr;
struct UnaryExpr;

struct ExpressionStmt;
struct VarStmt;

using LiteralValue = std::variant<std::monostate, std::string, long long, double, bool>;

class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;
    virtual void visit(const std::shared_ptr<BinaryExpr>& expr) = 0;
    virtual void visit(const std::shared_ptr<GroupingExpr>& expr) = 0;
    virtual void visit(const std::shared_ptr<LiteralExpr>& expr) = 0;
    virtual void visit(const std::shared_ptr<UnaryExpr>& expr) = 0;
};

class Expr {
public:
    virtual ~Expr() = default;
    virtual void accept(ExprVisitor& visitor) = 0;
};

struct BinaryExpr : Expr {
    std::shared_ptr<Expr> left;
    Token op;
    std::shared_ptr<Expr> right;

    BinaryExpr(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
        : left(left), op(op), right(right) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(std::make_shared<BinaryExpr>(*this));
    }
};

struct GroupingExpr : Expr {
    std::shared_ptr<Expr> expression;

    GroupingExpr(std::shared_ptr<Expr> expression) : expression(expression) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(std::make_shared<GroupingExpr>(*this));
    }
};

struct LiteralExpr : Expr {
    LiteralValue value;

    LiteralExpr(LiteralValue value) : value(value) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(std::make_shared<LiteralExpr>(*this));
    }
};

struct UnaryExpr : Expr {
    Token op;
    std::shared_ptr<Expr> right;

    UnaryExpr(Token op, std::shared_ptr<Expr> right) : op(op), right(right) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(std::make_shared<UnaryExpr>(*this));
    }
};

class StmtVisitor {
public:
    virtual ~StmtVisitor() = default;
    virtual void visit(const std::shared_ptr<ExpressionStmt>& stmt) = 0;
    virtual void visit(const std::shared_ptr<VarStmt>& stmt) = 0;
};

class Stmt {
public:
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor& visitor) = 0;
};

struct ExpressionStmt : Stmt {
    std::shared_ptr<Expr> expression;

    ExpressionStmt(std::shared_ptr<Expr> expression) : expression(expression) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visit(std::make_shared<ExpressionStmt>(*this));
    }
};

struct VarStmt : Stmt {
    Token name;
    std::shared_ptr<Expr> initializer;

    VarStmt(Token name, std::shared_ptr<Expr> initializer)
        : name(name), initializer(initializer) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visit(std::make_shared<VarStmt>(*this));
    }
};
