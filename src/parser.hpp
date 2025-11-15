#pragma once

#include "token.hpp"
#include "ast.hpp"
#include <vector>

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::vector<std::unique_ptr<Stmt>> parse();

private:
    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> function(const std::string& kind);
    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> if_statement();
    std::unique_ptr<Stmt> while_statement();
    std::unique_ptr<Stmt> for_statement();
    std::unique_ptr<Stmt> return_statement();
    std::unique_ptr<Stmt> block();
    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> assignment();
    std::unique_ptr<TypeExpr> type_expression();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> call();
    std::unique_ptr<Expr> primary();
    std::unique_ptr<Expr> finish_call(std::unique_ptr<Expr> callee);
    std::unique_ptr<Expr> finish_subscript(std::unique_ptr<Expr> callee);

    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);
    bool is_at_end();
    Token advance();
    Token peek();

    const std::vector<Token>& tokens_;
    int current_ = 0;
};
