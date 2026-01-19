#pragma once

#include <string>
#include <iostream>

enum class TokenType {
    // End of File
    EOF_TOKEN,

    // Identifiers
    IDENTIFIER,

    // Keywords
    KEYWORD_ITEM,
    KEYWORD_APP,
    KEYWORD_ROW,
    KEYWORD_COL,
    KEYWORD_RECT,
    KEYWORD_TEXT,
    KEYWORD_PROPERTY,
    KEYWORD_IMPORT,
    KEYWORD_EXPORT,
    KEYWORD_STRUCT,
    KEYWORD_STATE,
    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_DELEGATE,
    KEYWORD_FOR,
    KEYWORD_PACKAGE,

    // Literals
    STRING_LITERAL,
    NUMBER_LITERAL,
    HEX_COLOR_LITERAL,

    // Symbols
    LBRACE,     // {
    RBRACE,     // }
    LPAREN,     // (
    RPAREN,     // )
    LBRACKET,   // [
    RBRACKET,   // ]
    COLON,      // :
    SEMICOLON,  // ;
    DOT,        // .
    COMMA,      // ,

    // Operators
    PLUS,       // +
    MINUS,      // -
    STAR,       // *
    SLASH,      // /
    EQUAL,      // =
    LESS,       // <
    GREATER,    // >
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;

    friend std::ostream& operator<<(std::ostream& os, const Token& token) {
        os << "Token(" << static_cast<int>(token.type) << ", \"" << token.value << "\", "
           << token.line << ":" << token.column << ")";
        return os;
    }
};
