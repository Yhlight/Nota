#pragma once

#include "AST.h"
#include "Lexer.h"
#include <vector>

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);

    Program parse();

private:
    Token peek() const;
    Token advance();
    bool is_at_end() const;

    std::unique_ptr<Stmt> parse_statement();
    std::unique_ptr<Stmt> parse_let_statement();
    std::unique_ptr<Expr> parse_expression();
    std::unique_ptr<Expr> parse_primary_expression();

    std::vector<Token> tokens;
    size_t position = 0;
};
