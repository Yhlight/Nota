#ifndef PARSER_H
#define PARSER_H

#include "Token.h"
#include "Expr.h"
#include <vector>
#include <stdexcept>


class ParseError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<Expr> parse();

private:
    std::vector<Token> tokens;
    int current = 0;

    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();

    bool isAtEnd();
    Token peek();
    Token previous();
    Token advance();
    bool check(TokenType type);
    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);
    ParseError error(const Token& token, const std::string& message);
};

#endif // PARSER_H
