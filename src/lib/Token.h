#pragma once

#include <string>
#include <variant>

namespace nota {

enum class TokenType {
    // Keywords
    LET, MUT, FN, CLASS, IF, ELSE, WHILE, FOR, RETURN, TRUE, FALSE, END,

    // Literals
    IDENTIFIER, INTEGER, FLOAT, STRING,

    // Operators
    PLUS, MINUS, STAR, SLASH, PERCENT,
    ASSIGN, EQUALS, NOT_EQUALS, NOT,
    LESS, GREATER, LESS_EQUALS, GREATER_EQUALS,
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET,
    COMMA, COLON, SEMICOLON,

    // End of File
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string lexeme;
    std::variant<std::monostate, int, double, std::string, bool> literal;
    int line;
};

} // namespace nota
