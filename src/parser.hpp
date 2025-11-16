#ifndef NOTA_PARSER_HPP
#define NOTA_PARSER_HPP

#include "token.hpp"
#include "ast.hpp"
#include <vector>
#include <memory>

namespace nota {

    class Parser {
    public:
        Parser(const std::vector<Token>& tokens);
        std::vector<std::unique_ptr<Stmt>> parse();

    private:
        std::unique_ptr<Stmt> declaration();
        std::unique_ptr<Stmt> varDeclaration(bool isMutable);
        std::unique_ptr<Stmt> statement();
        std::unique_ptr<Stmt> ifStatement();
        std::unique_ptr<Stmt> whileStatement();
        std::unique_ptr<Stmt> doWhileStatement();
        std::unique_ptr<Stmt> expressionStatement();
        std::unique_ptr<Expr> expression();
        std::unique_ptr<Expr> assignment();
        std::unique_ptr<Expr> equality();
        std::unique_ptr<Expr> comparison();
        std::unique_ptr<Expr> term();
        std::unique_ptr<Expr> factor();
        std::unique_ptr<Expr> unary();
        std::unique_ptr<Expr> primary();

        bool match(const std::vector<TokenType>& types);
        Token consume(TokenType type, const std::string& message);
        bool check(TokenType type);
        Token advance();
        bool isAtEnd();
        Token peek();
        Token previous();
        void synchronize();

        const std::vector<Token>& tokens;
        int current = 0;
    };

} // namespace nota

#endif // NOTA_PARSER_HPP
