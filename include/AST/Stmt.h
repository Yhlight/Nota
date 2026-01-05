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
struct ImportStmt;
struct PackageStmt;
struct ExportStmt;

// Visitor Interface
class StmtVisitor {
public:
    virtual std::any visit(const ComponentStmt& stmt) = 0;
    virtual std::any visit(const PropertyStmt& stmt) = 0;
    virtual std::any visit(const ItemStmt& stmt) = 0;
    virtual std::any visit(const ImportStmt& stmt) = 0;
    virtual std::any visit(const PackageStmt& stmt) = 0;
    virtual std::any visit(const ExportStmt& stmt) = 0;
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

struct PackageStmt : Stmt {
    const std::unique_ptr<Expr> name;

    PackageStmt(std::unique_ptr<Expr> name) : name(std::move(name)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visit(*this);
    }
};

struct ExportStmt : Stmt {
    const std::vector<std::unique_ptr<Expr>> exports;

    ExportStmt(std::vector<std::unique_ptr<Expr>> exports) : exports(std::move(exports)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visit(*this);
    }
};

struct ImportStmt : Stmt {
    const std::unique_ptr<Expr> path;

    ImportStmt(std::unique_ptr<Expr> path) : path(std::move(path)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visit(*this);
    }
};

struct ItemStmt : Stmt {
    const Token name;
    const std::vector<std::unique_ptr<Stmt>> body;

    ItemStmt(Token name, std::vector<std::unique_ptr<Stmt>> body)
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
