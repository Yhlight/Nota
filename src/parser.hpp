#pragma once

#include "ast.hpp"
#include "token.hpp"
#include <vector>
#include <stdexcept>
#include <memory>

class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& message) : std::runtime_error(message) {}
};

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::vector<std::shared_ptr<Stmt>> parse();

private:
    std::vector<Token> tokens;
    int current = 0;

    bool isAtEnd();
    Token peek();
    Token previous();
    Token advance();
    bool check(TokenType type);
    template<typename... Args>
    bool match(Args... types);

    // Grammar rules
    std::shared_ptr<Stmt> declaration();
    std::shared_ptr<Stmt> statement();
    std::shared_ptr<Stmt> ifStatement();
    std::shared_ptr<Stmt> whileStatement();
    std::shared_ptr<Stmt> doWhileStatement();
    std::shared_ptr<Stmt> forStatement();
    std::shared_ptr<Stmt> varDeclaration();
    std::vector<std::shared_ptr<Stmt>> block();
    std::shared_ptr<Stmt> expressionStatement();
    std::shared_ptr<Expr> expression();
    std::shared_ptr<Expr> equality();
    std::shared_ptr<Expr> comparison();
    std::shared_ptr<Expr> term();
    std::shared_ptr<Expr> factor();
    std::shared_ptr<Expr> unary();
    std::shared_ptr<Expr> call();
    std::shared_ptr<Expr> primary();
    std::shared_ptr<Expr> finishCall(std::shared_ptr<Expr> callee);

    // Error handling
    Token consume(TokenType type, const std::string& message);
    ParseError error(Token token, const std::string& message);
    void synchronize();
};
