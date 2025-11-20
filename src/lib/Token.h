#pragma once

#include <string>
#include <variant>

namespace nota {

    enum class TokenType {
        // Single-character tokens
        LEFT_PAREN, RIGHT_PAREN,
        LEFT_BRACE, RIGHT_BRACE,
        LEFT_BRACKET, RIGHT_BRACKET,
        COMMA, DOT, MINUS, PLUS,
        SLASH, STAR, MODULO,
        COLON,

        // One or two character tokens
        BANG, BANG_EQUAL,
        EQUAL, EQUAL_EQUAL,
        GREATER, GREATER_EQUAL,
        LESS, LESS_EQUAL,
        PLUS_EQUAL, MINUS_EQUAL,
        STAR_EQUAL, SLASH_EQUAL,
        MODULO_EQUAL,
        PLUS_PLUS, MINUS_MINUS,

        // Literals
        IDENTIFIER, STRING,
        INTEGER, FLOAT,

        // Keywords
        LET, MUT,
        IF, ELSE, END,
        MATCH,
        WHILE, FOR, DO,
        FN, RETURN,
        CLASS, NEW, THIS,
        IMPORT, AS, PACKAGE,
        NONE, TRUE, FALSE,

        // Misc
        NEWLINE,
        END_OF_FILE,

        // Comments
        SINGLE_LINE_COMMENT,
        MULTI_LINE_COMMENT,

        // Errors
        UNKNOWN
    };

    struct Token {
        TokenType type;
        std::string lexeme;
        int line;
        int column;
    };

} // namespace nota
