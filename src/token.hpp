#pragma once

#include <string>
#include <vector>

enum class TokenType {
    // Keywords
    LET, MUT, INT_TYPE, FLOAT_TYPE, BOOL_TYPE, STRING_TYPE,
    IF, ELSE, END, MATCH, WHILE, FOR, DO, FUNC, RETURN, CLASS, NEW, THIS,
    IMPORT, AS, PACKAGE,

    // Literals
    IDENTIFIER, INTEGER, FLOAT, STRING, TRUE, FALSE,

    // Operators
    ASSIGN, PLUS, MINUS, STAR, SLASH, COLON, DOT, ARROW,
    EQUAL, NOT_EQUAL, LESS, GREATER, LESS_EQUAL, GREATER_EQUAL,

    // Punctuation
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET, COMMA,
    COLON_COLON,

    // Other
    UNEXPECTED, END_OF_FILE
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
};
