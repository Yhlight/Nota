#pragma once
#include <string>
#include <string_view>
#include <ostream>

enum class TokenType {
    Identifier,
    Number,
    String,
    LBrace,     // {
    RBrace,     // }
    Colon,      // :
    Semicolon,  // ;
    EndOfFile,
    Unknown
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;

    friend std::ostream& operator<<(std::ostream& os, const Token& token) {
        os << "Token(" << static_cast<int>(token.type) << ", \"" << token.value << "\", " << token.line << ":" << token.column << ")";
        return os;
    }
};
