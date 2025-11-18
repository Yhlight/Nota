#pragma once

#include "AST.h"
#include "Token.h"
#include <vector>
#include <memory>
#include <stdexcept>

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);
    std::vector<std::unique_ptr<Stmt>> parse();

private:
    // Simple error struct for now.
    struct ParseError : public std::runtime_error {
        using std::runtime_error::runtime_error;
    };

    std::vector<Token> tokens;
    int current = 0;

    // Parsing methods for statements
    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> varDeclaration();
    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> expressionStatement();

    // Parsing methods for different expression types
    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();

    // Helper methods
    bool match(const std::vector<TokenType>& types);
    bool check(TokenType type) const;
    Token advance();
    bool isAtEnd() const;
    Token peek() const;
    Token previous() const;
    Token consume(TokenType type, const std::string& message);
    ParseError error(const Token& token, const std::string& message);
};
