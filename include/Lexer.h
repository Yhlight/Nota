#ifndef NOTA_LEXER_H
#define NOTA_LEXER_H

#include "Token.h"
#include <string>

namespace nota {

    class Lexer {
    public:
        Lexer(const std::string& source);

        Token next_token();

    private:
        std::string source;
        int current;
        int line;
        int column;

        char peek();
        char peek_next();
        char advance();
        bool match(char expected);
        Token make_token(TokenType type, const std::string& lexeme);
    };

} // namespace nota

#endif // NOTA_LEXER_H
