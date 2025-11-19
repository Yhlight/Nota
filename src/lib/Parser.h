#pragma once

#include "Token.h"
#include "Expr.h"
#include <vector>
#include <memory>
#include <stdexcept>

#include "Lexer.h" // For NotaError

#include "Stmt.h"

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::vector<std::shared_ptr<Stmt>> parse();
    bool hasError() const;
    const std::vector<NotaError>& getErrors() const;

private:
    const std::vector<Token>& tokens;
    std::vector<NotaError> errors;
    int current = 0;

    std::shared_ptr<Stmt> declaration();
    std::shared_ptr<Stmt> varDeclaration(bool isMutable);
    std::shared_ptr<Stmt> statement();
    std::shared_ptr<Stmt> ifStatement();
    std::shared_ptr<Stmt> whileStatement();
    std::shared_ptr<Stmt> expressionStatement();
    std::shared_ptr<Expr> expression();
    void synchronize();
    std::shared_ptr<Expr> assignment();
    std::shared_ptr<Expr> equality();
    std::shared_ptr<Expr> comparison();
    std::shared_ptr<Expr> term();
    std::shared_ptr<Expr> factor();
    std::shared_ptr<Expr> unary();
    std::shared_ptr<Expr> primary();
    std::vector<std::shared_ptr<Stmt>> block();

    bool isAtEnd();
    Token peek();
    Token previous();
    Token advance();
    bool check(TokenType type);
    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);
    std::runtime_error error(const Token& token, const std::string& message);
};
