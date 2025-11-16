#pragma once

#include "AST.h"
#include "Lexer.h"
#include <vector>
#include <string>

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);

    Program parse();

private:
    Token peek() const;
    Token advance();
    bool is_at_end() const;
    Token consume(TokenType type, const std::string& message);

    std::unique_ptr<Stmt> parse_statement();
    std::unique_ptr<Stmt> parse_let_statement();
    std::unique_ptr<Stmt> parse_if_statement(bool is_else_if = false);
    std::unique_ptr<Stmt> parse_block_statement();
    std::unique_ptr<Expr> parse_expression(int min_precedence = 0);
    std::unique_ptr<Expr> parse_primary_expression();

    std::vector<Token> tokens;
    size_t position = 0;
};
