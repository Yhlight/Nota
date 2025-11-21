#ifndef NOTA_TOKEN_H
#define NOTA_TOKEN_H

#include <string>

namespace nota {

    enum class TokenType {
        // Single-character tokens.
        LEFT_PAREN, RIGHT_PAREN,
        LEFT_BRACE, RIGHT_BRACE,
        LEFT_BRACKET, RIGHT_BRACKET,
        COMMA, DOT, MINUS, PLUS,
        SEMICOLON, SLASH, STAR,

        // One or two character tokens.
        BANG, BANG_EQUAL,
        EQUAL, EQUAL_EQUAL,
        GREATER, GREATER_EQUAL,
        LESS, LESS_EQUAL,

        // Literals.
        IDENTIFIER, STRING, NUMBER,

        // Keywords.
        AND, CLASS, ELSE, FALSE,
        FN, FOR, IF, NIL, OR,
        PRINT, RETURN, SUPER, THIS,
        TRUE, LET, MUT, WHILE, END,

        END_OF_FILE
    };

    struct Token {
        TokenType type;
        std::string lexeme;
        int line;
    };

} // namespace nota

#endif // NOTA_TOKEN_H