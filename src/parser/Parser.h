#pragma once

#include "Token.h"
#include "Expr.h"
#include "Stmt.h"
#include <vector>
#include <memory>
#include <string>

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::vector<std::unique_ptr<Stmt>> parse();

private:
    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> var_declaration();
    std::unique_ptr<Stmt> print_statement();
    std::unique_ptr<Stmt> expression_statement();
    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();

    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);
    Token advance();
    bool is_at_end();
    Token peek();
    Token previous();

    const std::vector<Token>& tokens_;
    size_t current_ = 0;
};
