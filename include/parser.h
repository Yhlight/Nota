#pragma once
#include "lexer.h"
#include "ast.h"
#include <vector>
#include <memory>
#include <stdexcept>

namespace nota {

class Parser {
public:
    Parser(std::vector<Token> tokens);
    std::shared_ptr<Component> parse();

private:
    std::vector<Token> tokens;
    size_t pos;

    Token current();
    Token peek();
    void advance();
    void expect(TokenType type);

    std::shared_ptr<Component> parseComponent();
    std::shared_ptr<Property> parseProperty();
    std::shared_ptr<Expression> parseExpression();
};

}
