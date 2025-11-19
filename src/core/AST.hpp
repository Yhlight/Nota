#pragma once

#include "core/Token.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nota {
namespace core {

// Forward declarations
struct Statement;
struct Expression;

// Base class for all nodes in the AST
struct Node {
    virtual ~Node() = default;
    virtual std::string ToString() const = 0;
};

// Base class for all expression nodes
struct Expression : public Node {};

// Base class for all statement nodes
struct Statement : public Node {};

// The root node of the entire program
struct Program : public Node {
    std::vector<std::unique_ptr<Statement>> statements;

    std::string ToString() const override {
        std::string out;
        for (const auto& stmt : statements) {
            out += stmt->ToString();
        }
        return out;
    }
};

// Expression representing an identifier
struct Identifier : public Expression {
    Token token; // The IDENTIFIER token
    std::string value;

    std::string ToString() const override { return value; }
};

// Expression representing an integer literal
struct IntegerLiteral : public Expression {
    Token token; // The INTEGER token
    int64_t value;

    std::string ToString() const override { return std::to_string(value); }
};

// Expression for prefix operators (e.g., !, -)
struct PrefixExpression : public Expression {
    Token token; // The operator token (e.g., !, -)
    std::string op;
    std::unique_ptr<Expression> right;

    std::string ToString() const override {
        return "(" + op + right->ToString() + ")";
    }
};

// Expression for infix operators (e.g., +, -, *, /)
struct InfixExpression : public Expression {
    Token token; // The operator token (e.g., +)
    std::unique_ptr<Expression> left;
    std::string op;
    std::unique_ptr<Expression> right;

    std::string ToString() const override {
        return "(" + left->ToString() + " " + op + " " + right->ToString() + ")";
    }
};

// Statement for 'let' variable declarations
struct LetStatement : public Statement {
    Token token; // The LET token
    std::unique_ptr<Identifier> name;
    std::unique_ptr<Expression> value;

    std::string ToString() const override {
        std::string out(token.literal);
        out += " ";
        out += name->ToString();
        out += " = ";
        if (value) {
            out += value->ToString();
        }
        return out;
    }
};

// Statement for 'mut' variable declarations
struct MutStatement : public Statement {
    Token token; // The MUT token
    std::unique_ptr<Identifier> name;
    std::unique_ptr<Expression> value;

    std::string ToString() const override {
        std::string out(token.literal);
        out += " ";
        out += name->ToString();
        out += " = ";
        if (value) {
            out += value->ToString();
        }
        return out;
    }
};

// Statement wrapper for an expression
struct ExpressionStatement : public Statement {
    Token token; // The first token of the expression
    std::unique_ptr<Expression> expression;

    std::string ToString() const override {
        if (expression) {
            return expression->ToString();
        }
        return "";
    }
};

// A block of statements
struct BlockStatement : public Statement {
    Token token; // The first token of the block
    std::vector<std::unique_ptr<Statement>> statements;

    std::string ToString() const override {
        std::string out;
        for (const auto& s : statements) {
            out += s->ToString();
        }
        return out;
    }
};

// Statement for 'if...else' constructs
struct IfStatement : public Statement {
    Token token; // The 'if' token
    std::unique_ptr<Expression> condition;
    std::unique_ptr<BlockStatement> consequence;
    std::unique_ptr<Statement> alternative; // Can be another IfStatement or a BlockStatement

    std::string ToString() const override {
        std::string out;
        out += "if" + condition->ToString() + " " + consequence->ToString();
        if (alternative) {
            out += "else " + alternative->ToString();
        }
        return out;
    }
};

// Expression for assignment (e.g., a = 10)
struct AssignmentExpression : public Expression {
    Token token; // The '=' token
    std::unique_ptr<Identifier> name;
    std::unique_ptr<Expression> value;

    std::string ToString() const override {
        std::string out;
        out += name->ToString();
        out += " = ";
        out += value->ToString();
        return out;
    }
};

} // namespace core
} // namespace nota
