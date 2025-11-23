#pragma once

#include <string>
#include <variant>

namespace nota {

enum class TokenType {
    // Keywords
LET, MUT, FN, STATIC, CLASS, THIS, IF, ELSE, WHILE, FOR, DO, RETURN, TRUE, FALSE, END, IMPORT, AS, PACKAGE, NONE, MATCH,

    // Literals
    IDENTIFIER, INTEGER, FLOAT, STRING,

    // Operators
    PLUS, MINUS, STAR, SLASH, PERCENT,
    PLUS_PLUS,
    ASSIGN, PLUS_ASSIGN, MINUS_ASSIGN, STAR_ASSIGN, SLASH_ASSIGN, PERCENT_ASSIGN,
    EQUALS, NOT_EQUALS, NOT,
    LESS, GREATER, LESS_EQUALS, GREATER_EQUALS,
    AND, OR,
    AMPERSAND, PIPE, CARET, TILDE, LEFT_SHIFT, RIGHT_SHIFT,
    ARROW,
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET,
    COMMA, DOT, COLON, SEMICOLON, NEWLINE, COLON_COLON,

    // End of File
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string lexeme;
    std::variant<std::monostate, int, double, std::string, bool> literal;
    int line;
};

} // namespace nota
