#pragma once

#include <string>
#include <variant>

enum class TokenType {
    // Keywords
    LET, MUT, FUNC, CLASS, IF, ELSE, WHILE, FOR, RETURN, TRUE, FALSE,

    // Literals
    IDENTIFIER, INTEGER, FLOAT, STRING,

    // Operators
    PLUS, MINUS, ASTERISK, SLASH, PERCENT,
    ASSIGN, EQUALS, NOT_EQUALS,
    LESS, GREATER, LESS_EQUALS, GREATER_EQUALS,
    LOGICAL_AND, LOGICAL_OR, LOGICAL_NOT,

    // Punctuation
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET,
    COMMA, COLON, SEMICOLON,

    // End of File
    END_OF_FILE,

    // Illegal
    ILLEGAL
};

struct Token {
    TokenType type;
    std::string lexeme;
    std::variant<std::monostate, int, double, std::string> literal;
    int line;
};
