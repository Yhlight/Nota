#ifndef STMT_H
#define STMT_H

#include "Token.h"
#include "Expr.h"
#include <memory>
#include <vector>
#include <any>

class Expression;
class Print;
class Var;

template <typename R>
class StmtVisitor {
public:
    virtual R visitExpressionStmt(const Expression& stmt) = 0;
    virtual R visitPrintStmt(const Print& stmt) = 0;
    virtual R visitVarStmt(const Var& stmt) = 0;
    virtual ~StmtVisitor() = default;
};

class Stmt {
public:
    virtual ~Stmt() = default;
    virtual std::any accept(StmtVisitor<std::any>& visitor) const = 0;
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

#endif // STMT_H
