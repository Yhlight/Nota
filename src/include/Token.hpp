#pragma once

#include <string>

enum class TokenType {
    // Single-character tokens
    LeftBrace,    // {
    RightBrace,   // }
    Colon,        // :
    Semicolon,    // ;

    // Literals
    Identifier,   // e.g., App, width
    String,       // "hello"
    Number,       // 123, 10.5

    // End of file
    Eof,

    // Unknown token
    Unknown
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line {0};

    bool operator==(const Token& other) const {
        return type == other.type && lexeme == other.lexeme && line == other.line;
    }
};
