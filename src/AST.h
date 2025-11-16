#pragma once

#include "Lexer.h"
#include <memory>
#include <string>
#include <vector>

// Forward declarations
class Expr;
class Stmt;

// Base class for all expression nodes
class Expr {
public:
    virtual ~Expr() = default;
};

// Base class for all statement nodes
class Stmt {
public:
    virtual ~Stmt() = default;
};

// Expression for a numeric literal like "123" or "45.67"
class NumberExpr : public Expr {
public:
    explicit NumberExpr(Token number) : value(std::move(number)) {}

    Token value;
};

// Expression for a binary operation like "1 + 2"
class BinaryExpr : public Expr {
public:
    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;
};

// Statement for a variable declaration, e.g., "let x = 5;"
class VarDeclStmt : public Stmt {
public:
    VarDeclStmt(Token name, std::unique_ptr<Expr> initializer)
        : name(std::move(name)), initializer(std::move(initializer)) {}

    Token name;
    std::unique_ptr<Expr> initializer;
};

// Statement for a block of statements, e.g., "{ let x = 1; }"
class BlockStmt : public Stmt {
public:
    explicit BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
        : statements(std::move(statements)) {}

    std::vector<std::unique_ptr<Stmt>> statements;
};

// Statement for an if statement, e.g., "if (x) { ... } else { ... }"
class IfStmt : public Stmt {
public:
    IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> then_branch,
           std::unique_ptr<Stmt> else_branch)
        : condition(std::move(condition)),
          then_branch(std::move(then_branch)),
          else_branch(std::move(else_branch)) {}

    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> then_branch;
    std::unique_ptr<Stmt> else_branch;
};

// A program is a series of statements
using Program = std::vector<std::unique_ptr<Stmt>>;
