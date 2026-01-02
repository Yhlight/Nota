#ifndef NOTA_TOKEN_HPP
#define NOTA_TOKEN_HPP

#include <string>
#include <vector>

enum class TokenType {
    // Single-character tokens
    LEFT_BRACE, RIGHT_BRACE,
    LEFT_BRACKET, RIGHT_BRACKET,
    COLON, DOT, SEMICOLON, SLASH, STAR, EQUAL, PERCENT, HASH,

    // Literals
    IDENTIFIER, STRING, NUMBER,

    // Keywords
    APP, ROW, COL, RECT, TEXT, ITEM,
    PARENT, THIS,

    // End of file
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
};

#endif // NOTA_TOKEN_HPP
