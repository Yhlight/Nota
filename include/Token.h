#pragma once

#include "TokenType.h"
#include <string>
#include <any>

struct Token {
    TokenType type;
    std::string lexeme;
    std::any literal;
    int line;

    Token(TokenType type, std::string lexeme, std::any literal, int line)
        : type(type), lexeme(std::move(lexeme)), literal(std::move(literal)), line(line) {}

    std::string to_string() const {
        return "Token [type=" + ::to_string(type) + ", lexeme='" + lexeme + "']";
    }
};
