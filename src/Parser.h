#ifndef NOTA_PARSER_H
#define NOTA_PARSER_H

#include "Token.h"
#include "AST/Stmt.h"
#include <vector>
#include <memory>

namespace nota {

    class Parser {
        const std::vector<Token> tokens;
        int current = 0;

        bool is_at_end();
        Token peek();
        Token previous();
        Token advance();
        bool check(TokenType type);
        bool match(std::vector<TokenType> types);
        Token consume(TokenType type, const std::string& message);
        std::unique_ptr<ast::Stmt> declaration();

    public:
        explicit Parser(const std::vector<Token>& tokens);
        std::vector<std::unique_ptr<ast::Stmt>> parse();
    };

} // namespace nota

#endif //NOTA_PARSER_H
