#ifndef NOTA_AST_HPP
#define NOTA_AST_HPP

#include "Token.hpp"
#include <vector>
#include <memory>
#include <variant>
#include <any>

namespace Nota {

// Forward declare node types
struct BinaryExpr;
struct LiteralExpr;
struct GroupingExpr;
struct ComponentNode;
struct PropertyNode;

// Visitor interface for expressions
class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;
    virtual std::any visit(const BinaryExpr& expr) = 0;
    virtual std::any visit(const LiteralExpr& expr) = 0;
    virtual std::any visit(const GroupingExpr& expr) = 0;
};

// Base class for all expression nodes
class Expr {
public:
    virtual ~Expr() = default;
    virtual std::any accept(ExprVisitor& visitor) const = 0;
};

// Literal values (e.g., numbers, strings, colors)
struct LiteralValue : std::variant<std::string, double> {};

struct LiteralExpr : Expr {
    LiteralValue value;
    TokenType type;

    LiteralExpr(const std::string& val, TokenType t) : value(val), type(t) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visit(*this);
    }
};

// Binary expressions (e.g., parent.width / 2)
struct BinaryExpr : Expr {
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    BinaryExpr(std::unique_ptr<Expr> l, Token o, std::unique_ptr<Expr> r)
        : left(std::move(l)), op(std::move(o)), right(std::move(r)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visit(*this);
    }
};

// Grouping expressions (e.g., (5 + 5))
struct GroupingExpr : Expr {
    std::unique_ptr<Expr> expression;

    explicit GroupingExpr(std::unique_ptr<Expr> expr) : expression(std::move(expr)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visit(*this);
    }
};

// Visitor interface for AST nodes (components, properties)
class AstVisitor {
public:
    virtual ~AstVisitor() = default;
    virtual void visit(const ComponentNode& node) = 0;
    virtual void visit(const PropertyNode& node) = 0;
};

// Base class for all AST nodes
class AstNode {
public:
    virtual ~AstNode() = default;
    virtual void accept(AstVisitor& visitor) const = 0;
};

// Represents a property (e.g., width: 100)
struct PropertyNode : AstNode {
    Token name;
    std::unique_ptr<Expr> value;

    PropertyNode(Token n, std::unique_ptr<Expr> v) : name(std::move(n)), value(std::move(v)) {}

    void accept(AstVisitor& visitor) const override {
        visitor.visit(*this);
    }
};

// Represents a component (e.g., App { ... })
struct ComponentNode : AstNode {
    Token type;
    std::vector<std::unique_ptr<PropertyNode>> properties;
    std::vector<std::unique_ptr<ComponentNode>> children;

    explicit ComponentNode(Token t) : type(std::move(t)) {}

    void accept(AstVisitor& visitor) const override {
        visitor.visit(*this);
    }
};

} // namespace Nota

#endif // NOTA_AST_HPP
