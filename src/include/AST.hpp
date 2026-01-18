#ifndef NOTA_AST_HPP
#define NOTA_AST_HPP

#include "Token.hpp"
#include <memory>
#include <vector>
#include <string>

// Forward declarations
struct Expression;
struct Statement;
struct LiteralExpr;
struct IdentifierExpr;
struct PropertyStmt;
struct ComponentStmt;

// Visitor pattern for traversing the AST
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visit(const LiteralExpr& expr) = 0;
    virtual void visit(const IdentifierExpr& expr) = 0;
    virtual void visit(const PropertyStmt& stmt) = 0;
    virtual void visit(const ComponentStmt& stmt) = 0;
};

// Base class for all AST nodes
struct Node {
    virtual ~Node() = default;
    virtual void accept(ASTVisitor& visitor) const = 0;
};

// Base class for all expression nodes
struct Expression : Node {};

// Base class for all statement nodes
struct Statement : Node {};

// Represents a literal value (e.g., number, string)
struct LiteralExpr : Expression {
    Token value;
    LiteralExpr(Token value) : value(std::move(value)) {}

    void accept(ASTVisitor& visitor) const override { visitor.visit(*this); }
};

// Represents an identifier used as a value
struct IdentifierExpr : Expression {
    Token name;
    IdentifierExpr(Token name) : name(std::move(name)) {}

    void accept(ASTVisitor& visitor) const override { visitor.visit(*this); }
};

// Represents a property declaration (e.g., width: 100)
struct PropertyStmt : Statement {
    Token name;
    std::unique_ptr<Expression> value;

    PropertyStmt(Token name, std::unique_ptr<Expression> value)
        : name(std::move(name)), value(std::move(value)) {}

    void accept(ASTVisitor& visitor) const override { visitor.visit(*this); }
};

// Represents a component declaration (e.g., Row { ... })
struct ComponentStmt : Statement {
    Token type;
    std::vector<std::unique_ptr<Statement>> body;

    ComponentStmt(Token type, std::vector<std::unique_ptr<Statement>> body)
        : type(std::move(type)), body(std::move(body)) {}

    void accept(ASTVisitor& visitor) const override { visitor.visit(*this); }
};

// Represents the root of a Nota file
struct Program {
    std::vector<std::unique_ptr<Statement>> statements;
};

#endif // NOTA_AST_HPP
