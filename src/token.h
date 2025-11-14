#ifndef NOTA_TOKEN_H
#define NOTA_TOKEN_H

#include <string>

enum class TokenType {
    // Keywords
    LET,
    MUT,
    BOOL,

    // Identifiers and literals
    IDENTIFIER,
    INTEGER,
    FLOAT,
    STRING,

    // Operators
    ASSIGN,

    // Punctuation
    SEMICOLON, // Even though not used, it's good to have for error recovery
    COLON,

    // End of file
    END_OF_FILE,

    // Illegal token
    ILLEGAL
};

struct Token {
    TokenType type;
    std::string literal;
};

#endif //NOTA_TOKEN_H
