#pragma once

#include <string>
#include <variant>

enum class TokenType {
    // Single-character tokens
    PLUS, MINUS, STAR, SLASH, PERCENT,
    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACE, RIGHT_BRACE,
    LEFT_BRACKET, RIGHT_BRACKET,
    COMMA, DOT, COLON, SEMICOLON,

    // One or two character tokens
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,
    PLUS_PLUS, MINUS_MINUS,
    PLUS_EQUAL, MINUS_EQUAL,
    STAR_EQUAL, SLASH_EQUAL,

    // Literals
    IDENTIFIER, STRING, NUMBER_INT, NUMBER_FLOAT,

    // Keywords
    LET, MUT, FUNC, CLASS, IF, ELSE, END,
    MATCH, WHILE, FOR, DO, RETURN,
    TRUE, FALSE, NONE, IMPORT, AS, PACKAGE,

    // Special
    NEWLINE,
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string lexeme;
    std::variant<std::monostate, int, double, std::string, bool> literal;
    int line;
};
