#pragma once

#include "Token.h"
#include "Expr.h"
#include <vector>
#include <memory>
#include <stdexcept>

#include "Lexer.h" // For NotaError

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::shared_ptr<Expr> parse();
    bool hasError() const;
    const std::vector<NotaError>& getErrors() const;

private:
    const std::vector<Token>& tokens;
    std::vector<NotaError> errors;
    int current = 0;

    std::shared_ptr<Expr> expression();
    std::shared_ptr<Expr> equality();
    std::shared_ptr<Expr> comparison();
    std::shared_ptr<Expr> term();
    std::shared_ptr<Expr> factor();
    std::shared_ptr<Expr> unary();
    std::shared_ptr<Expr> primary();

    bool isAtEnd();
    Token peek();
    Token previous();
    Token advance();
    bool check(TokenType type);
    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);
    std::runtime_error error(const Token& token, const std::string& message);
};
