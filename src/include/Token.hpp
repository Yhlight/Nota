#ifndef NOTA_TOKEN_HPP
#define NOTA_TOKEN_HPP

#include <string>
#include <vector>

enum class TokenType {
    // Single-character tokens
    LEFT_BRACE, RIGHT_BRACE, COLON, LEFT_PAREN, RIGHT_PAREN, LEFT_BRACKET, RIGHT_BRACKET, COMMA, DOT, SLASH, PERCENT,

    // Literals
    IDENTIFIER, STRING, NUMBER,

    // Keywords
    APP, ROW, COL, RECT, TEXT, ITEM,
    PACKAGE, IMPORT, EXPORT, AS,
    BUTTON, DELEGATE, FOR,
    STATES, STATE, WHEN,
    IF, ELSE, PROPERTY,
    STRUCT,

    // Built-in Types
    TYPE_INT, TYPE_DOUBLE, TYPE_BOOL, TYPE_STRING, TYPE_VOID, TYPE_LIST,

    // Boolean Literals
    TRUE, FALSE,

    // Special Tokens
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
};

#endif // NOTA_TOKEN_HPP
