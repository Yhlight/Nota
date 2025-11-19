#pragma once

#include "Expr.h"
#include "Token.h"
#include <memory>
#include <vector>

// Forward declarations
struct ExpressionStmt;
struct VarStmt;

// Visitor interface
class StmtVisitor {
public:
    virtual ~StmtVisitor() = default;
    virtual void visit(const ExpressionStmt& stmt) = 0;
    virtual void visit(const VarStmt& stmt) = 0;
};

// Base class for all statement nodes
class Stmt {
public:
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor& visitor) const = 0;
};

// Concrete statement classes
struct ExpressionStmt : Stmt {
    explicit ExpressionStmt(std::shared_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const std::shared_ptr<Expr> expression;
};

struct VarStmt : Stmt {
    VarStmt(Token name, std::shared_ptr<Expr> initializer, bool isMutable)
        : name(std::move(name)), initializer(std::move(initializer)), isMutable(isMutable) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const Token name;
    const std::shared_ptr<Expr> initializer;
    const bool isMutable;
};
