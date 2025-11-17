#ifndef NOTA_PARSER_H
#define NOTA_PARSER_H

#include "Lexer.h"
#include "AST/Stmt.h"
#include <vector>
#include <map>

namespace nota {

    class Parser;

    typedef std::unique_ptr<ast::Expr> (Parser::*PrefixParseFn)();
    typedef std::unique_ptr<ast::Expr> (Parser::*InfixParseFn)(std::unique_ptr<ast::Expr>);

    enum Precedence {
        PREC_NONE,
        PREC_ASSIGNMENT,  // =
        PREC_OR,          // or
        PREC_AND,         // and
        PREC_EQUALITY,    // == !=
        PREC_COMPARISON,  // < > <= >=
        PREC_TERM,        // + -
        PREC_FACTOR,      // * /
        PREC_UNARY,       // ! -
        PREC_POSTFIX,     // ++ --
        PREC_CALL,        // . ()
        PREC_PRIMARY
    };

    struct ParseRule {
        PrefixParseFn prefix;
        InfixParseFn infix;
        Precedence precedence;
    };

    class Parser {
    public:
        Parser(Lexer& lexer);

        std::vector<std::unique_ptr<ast::Stmt>> parse();

    private:
        Lexer& lexer;
        Token current_token;
        Token previous_token;

        void advance();
        void consume(TokenType type, const std::string& message);
        bool match(TokenType type);

        std::unique_ptr<ast::Stmt> statement();
        std::unique_ptr<ast::Stmt> var_declaration();
        std::unique_ptr<ast::Stmt> if_statement();
        std::unique_ptr<ast::Stmt> while_statement();
        std::unique_ptr<ast::Stmt> do_while_statement();
        std::unique_ptr<ast::Stmt> for_statement();
        std::unique_ptr<ast::Stmt> match_statement();
        std::unique_ptr<ast::Stmt> func_declaration();
        std::unique_ptr<ast::Stmt> class_declaration();
        std::unique_ptr<ast::Stmt> expression_statement();
        std::unique_ptr<ast::Stmt> block();
        std::unique_ptr<ast::Expr> expression();

        // Pratt parser methods
        std::unique_ptr<ast::Expr> parse_precedence(Precedence precedence);
        ParseRule& get_rule(TokenType type);

        // Prefix parse functions
        std::unique_ptr<ast::Expr> unary();
        std::unique_ptr<ast::Expr> literal();
        std::unique_ptr<ast::Expr> grouping();
        std::unique_ptr<ast::Expr> variable();
        std::unique_ptr<ast::Expr> lambda_expression();
        std::unique_ptr<ast::Expr> array_literal();
        std::unique_ptr<ast::Expr> this_();

        // Infix parse functions
        std::unique_ptr<ast::Expr> binary(std::unique_ptr<ast::Expr> left);
        std::unique_ptr<ast::Expr> assignment(std::unique_ptr<ast::Expr> left);
        std::unique_ptr<ast::Expr> postfix(std::unique_ptr<ast::Expr> left);
        std::unique_ptr<ast::Expr> call(std::unique_ptr<ast::Expr> left);
        std::unique_ptr<ast::Expr> subscript(std::unique_ptr<ast::Expr> left);
        std::unique_ptr<ast::Expr> get(std::unique_ptr<ast::Expr> left);
        std::unique_ptr<ast::Expr> initializer_list(std::unique_ptr<ast::Expr> left);

        std::map<TokenType, ParseRule> rules;
    };

} // namespace nota

#endif // NOTA_PARSER_H
