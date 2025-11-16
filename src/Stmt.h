#ifndef STMT_H
#define STMT_H

#include "Token.h"
#include "Expr.h"
#include <memory>
#include <vector>
#include <any>

class Block;
class Expression;
class If;
class Print;
class Var;
class While;

template <typename R>
class StmtVisitor {
public:
    virtual R visitBlockStmt(const Block& stmt) = 0;
    virtual R visitExpressionStmt(const Expression& stmt) = 0;
    virtual R visitIfStmt(const If& stmt) = 0;
    virtual R visitPrintStmt(const Print& stmt) = 0;
    virtual R visitVarStmt(const Var& stmt) = 0;
    virtual R visitWhileStmt(const While& stmt) = 0;
    virtual ~StmtVisitor() = default;
};

class Stmt {
public:
    virtual ~Stmt() = default;
    virtual std::any accept(StmtVisitor<std::any>& visitor) const = 0;
};

class Block : public Stmt {
public:
    Block(std::vector<std::unique_ptr<Stmt>> statements) :
        statements(std::move(statements)) {}

    std::any accept(StmtVisitor<std::any>& visitor) const override {
        return visitor.visitBlockStmt(*this);
    }

    const std::vector<std::unique_ptr<Stmt>> statements;
};

class Expression : public Stmt {
public:
    Expression(std::unique_ptr<Expr> expression) :
        expression(std::move(expression)) {}

    std::any accept(StmtVisitor<std::any>& visitor) const override {
        return visitor.visitExpressionStmt(*this);
    }

    const std::unique_ptr<Expr> expression;
};

class If : public Stmt {
public:
    If(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch, std::unique_ptr<Stmt> elseBranch) :
        condition(std::move(condition)),
        thenBranch(std::move(thenBranch)),
        elseBranch(std::move(elseBranch)) {}

    std::any accept(StmtVisitor<std::any>& visitor) const override {
        return visitor.visitIfStmt(*this);
    }

    const std::unique_ptr<Expr> condition;
    const std::unique_ptr<Stmt> thenBranch;
    const std::unique_ptr<Stmt> elseBranch;
};

class Print : public Stmt {
public:
    Print(std::unique_ptr<Expr> expression) :
        expression(std::move(expression)) {}

    std::any accept(StmtVisitor<std::any>& visitor) const override {
        return visitor.visitPrintStmt(*this);
    }

    const std::unique_ptr<Expr> expression;
};

class Var : public Stmt {
public:
    Var(Token name, std::unique_ptr<Expr> initializer) :
        name(std::move(name)),
        initializer(std::move(initializer)) {}

    std::any accept(StmtVisitor<std::any>& visitor) const override {
        return visitor.visitVarStmt(*this);
    }

    const Token name;
    const std::unique_ptr<Expr> initializer;
};

class While : public Stmt {
public:
    While(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body) :
        condition(std::move(condition)),
        body(std::move(body)) {}

    std::any accept(StmtVisitor<std::any>& visitor) const override {
        return visitor.visitWhileStmt(*this);
    }

    const std::unique_ptr<Expr> condition;
    const std::unique_ptr<Stmt> body;
};

#endif // STMT_H
