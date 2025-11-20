#pragma once

#include "Token.h"
#include "AST.h"
#include <vector>
#include <memory>

namespace nota {

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::shared_ptr<Expr> parse();

private:
    std::shared_ptr<Expr> expression();
    std::shared_ptr<Expr> equality();
    std::shared_ptr<Expr> comparison();
    std::shared_ptr<Expr> term();
    std::shared_ptr<Expr> factor();
    std::shared_ptr<Expr> unary();
    std::shared_ptr<Expr> primary();

    bool match(const std::vector<TokenType>& types);
    Token advance();
    bool isAtEnd() const;
    Token peek() const;
    Token previous() const;
    Token consume(TokenType type, const std::string& message);


    std::vector<Token> tokens_;
    size_t current_ = 0;
};

} // namespace nota
