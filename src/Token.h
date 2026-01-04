#ifndef NOTA_TOKEN_H
#define NOTA_TOKEN_H

#include <string>
#include <utility>

namespace nota {

    enum class TokenType {
        // Single-character tokens.
        LEFT_BRACE, RIGHT_BRACE,
        COLON, SEMICOLON, DOT,
        PLUS, MINUS, STAR, SLASH,

        // Literals.
        IDENTIFIER, STRING, NUMBER,

        // Keywords.
        APP, ROW, COL, RECT, TEXT, ITEM,
        IF, ELSE,
        IMPORT, AS, PACKAGE, EXPORT,
        DELEGATE, FOR,
        STATES, STATE, WHEN,
        PROPERTY,
        TRUE, FALSE,

        // Special
        END_OF_FILE,
        UNKNOWN
    };

    struct Token {
        TokenType type;
        std::string lexeme;
        int line;

        Token(TokenType type, std::string lexeme, int line)
            : type(type), lexeme(std::move(lexeme)), line(line) {}
    };

} // namespace nota

#endif //NOTA_TOKEN_H
