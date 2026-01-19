#pragma once
#include "Lexer.h"
#include "AST.h"
#include <vector>

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);
    std::unique_ptr<Node> parse();
    std::vector<std::unique_ptr<Node>> parseAll();

private:
    std::vector<Token> tokens;
    size_t current = 0;

    const Token& peek(int offset = 0) const;
    const Token& previous() const;
    bool isAtEnd() const;
    Token advance();
    bool match(TokenType type);
    Token consume(TokenType type, const std::string& message);

    std::unique_ptr<Node> parseComponent();

    // Expressions
    std::unique_ptr<Expr> parseExpression();
    std::unique_ptr<Expr> parseEquality();
    std::unique_ptr<Expr> parseComparison();
    std::unique_ptr<Expr> parseTerm();
    std::unique_ptr<Expr> parseFactor();
    std::unique_ptr<Expr> parseUnary();
    std::unique_ptr<Expr> parsePrimary();
};
