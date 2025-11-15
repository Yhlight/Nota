#ifndef NOTA_PARSER_HPP
#define NOTA_PARSER_HPP

#include "ast.hpp"
#include "lexer.hpp"
#include <vector>
#include <memory>
#include <stdexcept>

namespace nota {

    class ParseError : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    class Parser {
    public:
        Parser(const std::vector<Token>& tokens);
        std::vector<std::unique_ptr<Stmt>> parse();

    private:
        std::unique_ptr<Stmt> declaration();
        std::unique_ptr<Stmt> function_declaration();
        std::unique_ptr<Stmt> var_declaration(bool is_mutable);
        std::unique_ptr<Stmt> statement();
        std::unique_ptr<Stmt> for_statement();
        std::unique_ptr<Stmt> if_statement();
        std::unique_ptr<Stmt> return_statement();
        std::unique_ptr<Stmt> while_statement();
        std::unique_ptr<Stmt> block();
        std::unique_ptr<Stmt> expression_statement();

        std::unique_ptr<Expr> expression();
        std::unique_ptr<Expr> assignment();
        std::unique_ptr<Expr> lambda_expression();
        std::unique_ptr<Expr> logical_or();
        std::unique_ptr<Expr> logical_and();
        std::unique_ptr<Expr> equality();
        std::unique_ptr<Expr> comparison();
        std::unique_ptr<Expr> term();
        std::unique_ptr<Expr> factor();
        std::unique_ptr<Expr> unary();
        std::unique_ptr<Expr> call();
        std::unique_ptr<Expr> primary();

        Token consume(TokenType type, const std::string& message);
        bool match(const std::vector<TokenType>& types);
        bool check(TokenType type);
        Token peek_next();
        Token advance();
        Token peek();
        Token previous();
        bool is_at_end();
        void synchronize();
        bool is_lambda();

        std::vector<Token> tokens;
        int current = 0;
    };

} // namespace nota

#endif // NOTA_PARSER_HPP
