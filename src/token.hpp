#ifndef NOTA_TOKEN_HPP
#define NOTA_TOKEN_HPP

#include <string>

namespace nota {

    enum class TokenType {
        // Single-character tokens
        LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
        LEFT_BRACKET, RIGHT_BRACKET, COMMA, DOT, MINUS,
        PLUS, SEMICOLON, SLASH, STAR, PERCENT, COLON,

        // One or two character tokens
        BANG, BANG_EQUAL,
        EQUAL, EQUAL_EQUAL,
        GREATER, GREATER_EQUAL,
        LESS, LESS_EQUAL,
        AMPERSAND, AMPERSAND_AMPERSAND,
        PIPE, PIPE_PIPE,
        PLUS_EQUAL, MINUS_EQUAL, STAR_EQUAL, SLASH_EQUAL,
        PERCENT_EQUAL, CARET, TILDE, LEFT_SHIFT, RIGHT_SHIFT,

        // Literals
        IDENTIFIER, STRING, NUMBER,

        // Keywords
        LET, MUT, TRUE, FALSE, IF, ELSE, WHILE, FOR, DO,
        FUNC, CLASS, RETURN, SUPER, THIS, IMPORT, AS,
        PACKAGE, MATCH, END,

        // Special
        ERROR, END_OF_FILE
    };

    struct Token {
        TokenType type;
        std::string lexeme;
        int line;
    };

} // namespace nota

#endif // NOTA_TOKEN_HPP
