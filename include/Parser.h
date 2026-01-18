#pragma once
#include "Lexer.h"
#include "AST.h"
#include <vector>

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);
    std::unique_ptr<ComponentNode> parse();
    std::vector<std::unique_ptr<ComponentNode>> parseAll();

private:
    std::vector<Token> tokens;
    size_t current = 0;

    const Token& peek(int offset = 0) const;
    const Token& previous() const;
    bool isAtEnd() const;
    bool match(TokenType type);
    Token consume(TokenType type, const std::string& message);

    std::unique_ptr<ComponentNode> parseComponent();
};
