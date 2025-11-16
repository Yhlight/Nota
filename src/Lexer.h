#pragma once

#include <string>
#include <vector>

enum class TokenType {
    // Keywords
    Let,
    Mut,
    If,
    Else,
    End,
    Match,

    // Literals
    Identifier,
    Number,

    // Operators
    Plus,
    Minus,
    Asterisk,
    Slash,
    Percent,
    Equal,
    LessThan,
    Colon,
    Comma,

    // Misc
    Unknown,
};

struct Token {
    TokenType type;
    std::string text;

    bool operator==(const Token& other) const {
        return type == other.type && text == other.text;
    }
};

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();
private:
    std::string source;
    size_t position = 0;
};
