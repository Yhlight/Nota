#pragma once

#include <vector>
#include <string>

enum class TokenType {
    // Single-character tokens.
    LEFT_BRACE, RIGHT_BRACE,
    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACKET, RIGHT_BRACKET,
    COLON, SEMICOLON, DOT, COMMA,
    MINUS, PLUS, SLASH, PERCENT, HASH,

    // Literals.
    IDENTIFIER,
    NUMBER,
    STRING,

    // Keywords (subset for now)
    IMPORT, AS, PACKAGE, EXPORT,
    IF, ELSE,
    TRUE, FALSE,

    // Events & Properties
    ON_CLICK,
    PROPERTY,

    // Special
    ERROR,
    END_OF_FILE
};

// Function to convert TokenType to string for debugging
inline std::string to_string(TokenType type) {
    static const std::vector<std::string> names = {
        "LEFT_BRACE", "RIGHT_BRACE",
        "LEFT_PAREN", "RIGHT_PAREN",
        "LEFT_BRACKET", "RIGHT_BRACKET",
        "COLON", "SEMICOLON", "DOT", "COMMA",
        "MINUS", "PLUS", "SLASH", "PERCENT", "HASH",
        "IDENTIFIER",
        "NUMBER",
        "STRING",
        "IMPORT", "AS", "PACKAGE", "EXPORT",
        "IF", "ELSE",
        "TRUE", "FALSE",
        "ON_CLICK",
        "PROPERTY",
        "ERROR",
        "END_OF_FILE"
    };
    return names[static_cast<int>(type)];
}
