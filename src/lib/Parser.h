#ifndef NOTA_PARSER_H
#define NOTA_PARSER_H

#include "Expr.h"
#include "Token.h"
#include <vector>
#include <memory>

namespace nota {

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<Expr> parse();

private:
    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();

    bool match(const std::vector<TokenType>& types);
    bool check(TokenType type);
    Token advance();
    bool isAtEnd();
    Token peek();
    Token previous();

    const std::vector<Token>& tokens;
    int current = 0;
};

} // namespace nota

#endif // NOTA_PARSER_H
