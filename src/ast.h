#ifndef NOTA_AST_H
#define NOTA_AST_H

#include "token.h"
#include <vector>
#include <memory>
#include <string>

// Base class for all nodes
class Node {
public:
    virtual ~Node() = default;
    virtual std::string String() const = 0;
};


// Base class for all expression nodes
class Expression : public Node {
public:
};

// Base class for all statement nodes
class Statement : public Node {
public:
};

class Program : public Node {
public:
    std::vector<std::unique_ptr<Statement>> statements;

    std::string String() const override {
        std::string out;
        for (const auto& s : statements) {
            out += s->String();
        }
        return out;
    }
};


class LetStatement : public Statement {
public:
    Token token; // The 'let' or 'mut' token
    Token name;
    std::unique_ptr<Expression> value;

    std::string String() const override {
        std::string out = token.literal + " " + name.literal + " = ";
        if (value) {
            out += value->String();
        }
        out += ";"; // using semicolon for string representation for now
        return out;
    }
};

// Represents an identifier
class Identifier : public Expression {
public:
    Token token;
    std::string value;

    std::string String() const override {
        return value;
    }
};

// Represents a literal value
class ExpressionLiteral : public Expression {
public:
    Token token;
    std::string String() const override { return token.literal; }
};

#endif //NOTA_AST_H
