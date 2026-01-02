#pragma once

#include <string>

enum class TokenType {
    // Single-character tokens
    LEFT_BRACE, RIGHT_BRACE,
    COLON, SEMICOLON,
    DOT, LEFT_BRACKET, RIGHT_BRACKET,
    EQUAL,

    // Literals
    IDENTIFIER, STRING, NUMBER,

    // Keywords
    ITEM,

    // Special
    PERCENT,

    // Comments
    SLASH, STAR,

    // Other
    UNEXPECTED,
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string_view text;
    int line;
    int column;
};
