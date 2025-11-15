#ifndef NOTA_AST_HPP
#define NOTA_AST_HPP

#include "token.hpp"
#include <memory>
#include <vector>
#include <variant>
#include <any>

namespace nota {

// Forward declarations
struct Expr;
struct BinaryExpr;
struct UnaryExpr;
struct LiteralExpr;
struct GroupingExpr;
struct VariableExpr;
struct AssignExpr;

struct Stmt;
struct ExpressionStmt;
struct VarDeclStmt;
struct BlockStmt;

// Visitor for Expressions
struct ExprVisitor {
    virtual ~ExprVisitor() = default;
    virtual std::any visit(const BinaryExpr& expr) = 0;
    virtual std::any visit(const UnaryExpr& expr) = 0;
    virtual std::any visit(const LiteralExpr& expr) = 0;
    virtual std::any visit(const GroupingExpr& expr) = 0;
    virtual std::any visit(const VariableExpr& expr) = 0;
    virtual std::any visit(const AssignExpr& expr) = 0;
};

// Base class for all expression nodes
struct Expr {
    virtual ~Expr() = default;
    virtual std::any accept(ExprVisitor& visitor) const = 0;
};

// Visitor for Statements
struct StmtVisitor {
    virtual ~StmtVisitor() = default;
    virtual void visit(const ExpressionStmt& stmt) = 0;
    virtual void visit(const VarDeclStmt& stmt) = 0;
    virtual void visit(const BlockStmt& stmt) = 0;
};

// Base class for all statement nodes
struct Stmt {
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor& visitor) const = 0;
};

// Expression Nodes
struct BinaryExpr : Expr {
    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visit(*this);
    }

    const std::unique_ptr<Expr> left;
    const Token op;
    const std::unique_ptr<Expr> right;
};

struct UnaryExpr : Expr {
    UnaryExpr(Token op, std::unique_ptr<Expr> right)
        : op(op), right(std::move(right)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visit(*this);
    }

    const Token op;
    const std::unique_ptr<Expr> right;
};

using LiteralValue = std::variant<std::monostate, bool, double, std::string>;

struct LiteralExpr : Expr {
    LiteralExpr(LiteralValue value) : value(value) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visit(*this);
    }

    const LiteralValue value;
};

struct GroupingExpr : Expr {
    GroupingExpr(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visit(*this);
    }

    const std::unique_ptr<Expr> expression;
};

struct VariableExpr : Expr {
    VariableExpr(Token name) : name(name) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visit(*this);
    }

    const Token name;
};

struct AssignExpr : Expr {
    AssignExpr(Token name, std::unique_ptr<Expr> value)
        : name(name), value(std::move(value)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visit(*this);
    }

    const Token name;
    const std::unique_ptr<Expr> value;
};


// Statement Nodes
struct ExpressionStmt : Stmt {
    ExpressionStmt(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const std::unique_ptr<Expr> expression;
};

struct VarDeclStmt : Stmt {
    VarDeclStmt(Token name, std::unique_ptr<Expr> initializer, bool isMutable)
        : name(name), initializer(std::move(initializer)), isMutable(isMutable) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const Token name;
    const std::unique_ptr<Expr> initializer;
    const bool isMutable;
};

struct BlockStmt : Stmt {
    BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
        : statements(std::move(statements)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const std::vector<std::unique_ptr<Stmt>> statements;
};

} // namespace nota

#endif // NOTA_AST_HPP
