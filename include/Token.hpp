#pragma once

#include <string>
#include <vector>

enum class TokenType {
    // Single-character tokens
    LEFT_BRACE, RIGHT_BRACE,
    LEFT_PAREN, RIGHT_PAREN,
    COLON, SEMICOLON, DOT, COMMA,

    // Literals
    IDENTIFIER,
    STRING,
    NUMBER,

    // Keywords (to be added later)

    // Miscellaneous
    UNEXPECTED,
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
};
