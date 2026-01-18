#pragma once

#include "lexer.h"
#include "ast.h"
#include <vector>
#include <memory>

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::shared_ptr<ComponentNode> parse();

private:
    std::vector<Token> tokens;
    size_t pos;

    Token peek(int offset = 0) const;
    Token advance();
    bool match(TokenType type);
    Token consume(TokenType type, const std::string& message);

    std::shared_ptr<ASTNode> parseStatement();
    std::shared_ptr<ComponentNode> parseComponent();
    std::shared_ptr<PropertyNode> parseProperty();
    std::shared_ptr<ASTNode> parseValue();
};
