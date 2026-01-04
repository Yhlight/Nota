#pragma once

#include "../Token.h"
#include "Expr.h"
#include <any>
#include <memory>
#include <vector>

// Forward declarations
struct ComponentStmt;
struct PropertyStmt;
struct ItemStmt;

// Visitor Interface
class StmtVisitor {
public:
    virtual std::any visit(const ComponentStmt& stmt) = 0;
    virtual std::any visit(const PropertyStmt& stmt) = 0;
    virtual std::any visit(const ItemStmt& stmt) = 0;
    virtual ~StmtVisitor() = default;
};

// Base class
class Stmt {
public:
    virtual std::any accept(StmtVisitor& visitor) const = 0;
    virtual ~Stmt() = default;
};

// Concrete classes
struct ComponentStmt : Stmt {
    const Token name;
    const std::vector<std::unique_ptr<Stmt>> body;

    ComponentStmt(Token name, std::vector<std::unique_ptr<Stmt>> body)
        : name(std::move(name)), body(std::move(body)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visit(*this);
    }
};

struct ItemStmt : Stmt {
    const Token name;
    const std::unique_ptr<Stmt> body;

    ItemStmt(Token name, std::unique_ptr<Stmt> body)
        : name(std::move(name)), body(std::move(body)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visit(*this);
    }
};

struct PropertyStmt : Stmt {
    const Token name;
    const std::unique_ptr<Expr> value;

    PropertyStmt(Token name, std::unique_ptr<Expr> value)
        : name(std::move(name)), value(std::move(value)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visit(*this);
    }
};
