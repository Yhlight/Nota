#ifndef NOTA_PARSER_H
#define NOTA_PARSER_H

#include "Lexer.h"
#include "AST/Stmt.h"
#include <vector>

namespace nota {

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
        std::unique_ptr<ast::Expr> expression();
        std::unique_ptr<ast::Expr> primary();
    };

} // namespace nota

#endif // NOTA_PARSER_H
