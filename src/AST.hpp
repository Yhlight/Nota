#ifndef NOTA_AST_HPP
#define NOTA_AST_HPP

#include "Token.hpp"
#include <memory>
#include <vector>

// Forward declarations
struct Component;
struct Property;
struct Literal;

// Visitor interface for the expression types
struct ExprVisitor {
    virtual void visit(const Component& expr) = 0;
    virtual void visit(const Property& expr) = 0;
    virtual void visit(const Literal& expr) = 0;
    virtual ~ExprVisitor() = default;
};

// Base class for all expression nodes
struct Expr {
    virtual void accept(ExprVisitor& visitor) const = 0;
};

// Represents a literal value (e.g., number, string, identifier)
struct Literal : Expr {
    Token value;

    Literal(Token value) : value(std::move(value)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }
};

// Represents a property assignment (e.g., width: 100)
struct Property : Expr {
    Token name;
    std::unique_ptr<Expr> value;

    Property(Token name, std::unique_ptr<Expr> value)
        : name(std::move(name)), value(std::move(value)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }
};

// Represents a component block (e.g., Row { ... })
struct Component : Expr {
    Token name;
    std::vector<std::unique_ptr<Property>> properties;
    std::vector<std::unique_ptr<Component>> children;

    Component(Token name,
              std::vector<std::unique_ptr<Property>> properties,
              std::vector<std::unique_ptr<Component>> children)
        : name(std::move(name)),
          properties(std::move(properties)),
          children(std::move(children)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }
};

#endif // NOTA_AST_HPP
