#ifndef NOTA_PARSER_HPP
#define NOTA_PARSER_HPP

#include "AST.hpp"
#include "Token.hpp"
#include <vector>
#include <memory>
#include <stdexcept>

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<Component> parse();

private:
    std::unique_ptr<Component> component();
    std::unique_ptr<Property> property();
    std::unique_ptr<Expr> expression();

    bool isAtEnd();
    Token peek();
    Token previous();
    Token advance();
    bool check(TokenType type);
    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);

    const std::vector<Token>& tokens;
    int current = 0;
};

#endif // NOTA_PARSER_HPP
