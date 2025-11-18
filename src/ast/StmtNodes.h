#pragma once

#include "Stmt.h"
#include "Expr.h"
#include "Token.h"
#include "Visitor.h"
#include <memory>
#include <vector>

class VarDeclStmt : public Stmt {
public:
    VarDeclStmt(Token name, std::unique_ptr<Expr> initializer)
        : name(name), initializer(std::move(initializer)) {}

    std::any accept(Visitor& visitor) override {
        return visitor.visit(*this);
    }

    Token name;
    std::unique_ptr<Expr> initializer;
};

class PrintStmt : public Stmt {
public:
    PrintStmt(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    std::any accept(Visitor& visitor) override {
        return visitor.visit(*this);
    }

    std::unique_ptr<Expr> expression;
};

class ExprStmt : public Stmt {
public:
    ExprStmt(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    std::any accept(Visitor& visitor) override {
        return visitor.visit(*this);
    }

    std::unique_ptr<Expr> expression;
};
