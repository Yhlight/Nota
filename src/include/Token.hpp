#ifndef NOTA_TOKEN_HPP
#define NOTA_TOKEN_HPP

#include <string>

enum class TokenType {
    // Single-character tokens
    LEFT_BRACE, RIGHT_BRACE, LEFT_PAREN, RIGHT_PAREN, COLON, SEMICOLON,

    // Literals
    IDENTIFIER, STRING, NUMBER,

    // Keywords
    ITEM, APP, ROW, COL, RECT, TEXT, BUTTON,
    IMPORT, AS, PACKAGE, EXPORT,
    PROPERTY, IF, ELSE, STATES, STATE, WHEN, DELEGATE, FOR,
    STRUCT,

    // Other
    END_OF_FILE, UNKNOWN
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
};

#endif // NOTA_TOKEN_HPP
