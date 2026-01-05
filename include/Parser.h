#pragma once

#include "Token.h"
#include "AST/Stmt.h"
#include "AST/Expr.h"
#include <vector>
#include <memory>

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);
    std::vector<std::unique_ptr<Stmt>> parse();

private:
    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> importStatement();
    std::unique_ptr<Stmt> item_declaration();
    std::unique_ptr<Stmt> component_declaration();
    std::unique_ptr<Stmt> property_declaration();
    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> call();
    std::unique_ptr<Expr> primary();

    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);
    bool check(TokenType type);
    Token advance();
    bool is_at_end();
    Token peek();
    Token previous();

    void synchronize();

    const std::vector<Token> tokens;
    int current = 0;
};
