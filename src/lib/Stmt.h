#pragma once

#include "Expr.h"
#include "Token.h"
#include <memory>
#include <vector>

// Forward declarations
struct ExpressionStmt;
struct VarStmt;
struct BlockStmt;
struct IfStmt;
struct WhileStmt;
struct ForStmt;
struct DoWhileStmt;
struct FunctionStmt;
struct ReturnStmt;

// Visitor interface
class StmtVisitor {
public:
    virtual ~StmtVisitor() = default;
    virtual void visit(const ExpressionStmt& stmt) = 0;
    virtual void visit(const VarStmt& stmt) = 0;
    virtual void visit(const BlockStmt& stmt) = 0;
    virtual void visit(const IfStmt& stmt) = 0;
    virtual void visit(const WhileStmt& stmt) = 0;
    virtual void visit(const ForStmt& stmt) = 0;
    virtual void visit(const DoWhileStmt& stmt) = 0;
    virtual void visit(const FunctionStmt& stmt) = 0;
    virtual void visit(const ReturnStmt& stmt) = 0;
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

struct BlockStmt : Stmt {
    BlockStmt(std::vector<std::shared_ptr<Stmt>> statements)
        : statements(std::move(statements)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const std::vector<std::shared_ptr<Stmt>> statements;
};

struct IfStmt : Stmt {
    IfStmt(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> thenBranch, std::shared_ptr<Stmt> elseBranch)
        : condition(std::move(condition)),
          thenBranch(std::move(thenBranch)),
          elseBranch(std::move(elseBranch)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const std::shared_ptr<Expr> condition;
    const std::shared_ptr<Stmt> thenBranch;
    const std::shared_ptr<Stmt> elseBranch;
};

struct WhileStmt : Stmt {
    WhileStmt(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body)
        : condition(std::move(condition)),
          body(std::move(body)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const std::shared_ptr<Expr> condition;
    const std::shared_ptr<Stmt> body;
};

struct ForStmt : Stmt {
    ForStmt(std::shared_ptr<Stmt> initializer, std::shared_ptr<Expr> condition, std::shared_ptr<Expr> increment, std::shared_ptr<Stmt> body)
        : initializer(std::move(initializer)),
          condition(std::move(condition)),
          increment(std::move(increment)),
          body(std::move(body)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const std::shared_ptr<Stmt> initializer;
    const std::shared_ptr<Expr> condition;
    const std::shared_ptr<Expr> increment;
    const std::shared_ptr<Stmt> body;
};

struct DoWhileStmt : Stmt {
    DoWhileStmt(std::shared_ptr<Stmt> body, std::shared_ptr<Expr> condition)
        : body(std::move(body)),
          condition(std::move(condition)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const std::shared_ptr<Stmt> body;
    const std::shared_ptr<Expr> condition;
};

struct FunctionStmt : Stmt {
    FunctionStmt(Token name, std::vector<Token> params, std::vector<std::shared_ptr<Stmt>> body)
        : name(std::move(name)), params(std::move(params)), body(std::move(body)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const Token name;
    const std::vector<Token> params;
    const std::vector<std::shared_ptr<Stmt>> body;
};

struct ReturnStmt : Stmt {
    ReturnStmt(Token keyword, std::shared_ptr<Expr> value)
        : keyword(std::move(keyword)), value(std::move(value)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const Token keyword;
    const std::shared_ptr<Expr> value;
};
