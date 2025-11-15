#ifndef NOTA_LEXER_HPP
#define NOTA_LEXER_HPP

#include "token.hpp"
#include <string>

namespace nota {

    class Lexer {
    public:
        Lexer(const std::string& source);
        Token scanToken();

    private:
        char advance();
        void skipWhitespace();
        Token string();
        Token number();
        Token identifier();
        TokenType identifierType();
        bool isAtEnd();
        char peek();
        char peekNext();
        bool match(char expected);
        Token makeToken(TokenType type) const;
        Token makeToken(TokenType type, const std::string& lexeme) const;
        Token errorToken(const std::string& message) const;

        const std::string& source;
        int start = 0;
        int current = 0;
        int line = 1;
    };

} // namespace nota

#endif // NOTA_LEXER_HPP
