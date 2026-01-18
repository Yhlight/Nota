#pragma once
#include "Lexer.h"
#include "AST.h"
#include <vector>

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);
    std::unique_ptr<ComponentNode> parse();

private:
    std::vector<Token> tokens;
    size_t current = 0;

    const Token& peek() const;
    const Token& previous() const;
    bool isAtEnd() const;
    bool match(TokenType type);
    Token consume(TokenType type, const std::string& message);
};
