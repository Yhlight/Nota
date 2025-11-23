/**
 * @file Parser.h
 * @brief The recursive descent parser for the Nota language.
 */

#pragma once

#include "Token.h"
#include "AST.h"
#include <vector>
#include <memory>
#include <stdexcept>

namespace nota {

/**
 * @class Parser
 * @brief The recursive descent parser for the Nota language.
 */
class Parser {
public:
    /**
     * @brief Construct a new Parser.
     * @param tokens The tokens to parse.
     */
    Parser(const std::vector<Token>& tokens);

    /**
     * @brief Parse the tokens.
     * @return A vector of statements.
     */
    std::vector<std::shared_ptr<Stmt>> parse();

    /**
     * @class ParseError
     * @brief An exception thrown when a parse error occurs.
     */
    class ParseError : public std::runtime_error {
    public:
        ParseError(const std::string& message) : std::runtime_error(message) {}
    };

    /**
     * @brief Check if the parser encountered an error.
     * @return True if an error was encountered, false otherwise.
     */
    bool hadError() const { return hadError_; }

private:
    std::shared_ptr<Expr> expression();
    std::shared_ptr<Expr> assignment();
    std::shared_ptr<Expr> logicOr();
    std::shared_ptr<Expr> logicAnd();
    std::shared_ptr<Expr> bitwiseOr();
    std::shared_ptr<Expr> bitwiseXor();
    std::shared_ptr<Expr> bitwiseAnd();
    std::shared_ptr<Expr> equality();
    std::shared_ptr<Expr> comparison();
    std::shared_ptr<Expr> shift();
    std::shared_ptr<Expr> term();
    std::shared_ptr<Expr> factor();
    std::shared_ptr<Expr> unary();
    std::shared_ptr<Expr> call();
    std::shared_ptr<Expr> primary();

    std::shared_ptr<Stmt> declaration();
    std::shared_ptr<Stmt> classDeclaration();
    std::shared_ptr<Stmt> functionDeclaration(bool is_static = false);
    std::shared_ptr<Stmt> statement();
    std::shared_ptr<Stmt> varDeclaration();
    std::shared_ptr<Stmt> importStatement();
    std::shared_ptr<Stmt> packageStatement();
    std::shared_ptr<Stmt> expressionStatement();
    std::shared_ptr<Stmt> ifStatement();
    std::shared_ptr<Stmt> matchStatement();
    std::shared_ptr<Stmt> whileStatement();
    std::shared_ptr<Stmt> forStatement();
    std::shared_ptr<Stmt> doWhileStatement();
    std::shared_ptr<Stmt> returnStatement();
    std::vector<std::shared_ptr<Stmt>> block();
    std::shared_ptr<Expr> finishCall(std::shared_ptr<Expr> callee);
    std::shared_ptr<TypeExpr> type();


    bool match(const std::vector<TokenType>& types);
    Token advance();
    bool isAtEnd() const;
    Token peek() const;
    Token previous() const;
    Token consume(TokenType type, const std::string& message);
    void consumeTerminators();
    void skipNewlines();
    ParseError error(const Token& token, const std::string& message);


    std::vector<Token> tokens_;
    size_t current_ = 0;
    bool hadError_ = false;
};

} // namespace nota
