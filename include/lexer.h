#pragma once
#include <string>
#include <vector>

namespace nota {

enum class TokenType {
    Identifier,
    Number,
    String,
    Colon,
    LBrace,
    RBrace,
    Percent, // For 100%
    Semicolon,
    Plus,
    Minus,
    Star,
    Slash,
    Dot,
    LParen,
    RParen,
    EndOfFile,
    Unknown
};

struct Token {
    TokenType type;
    std::string text;
    int line;
    int column;
};

class Lexer {
public:
    Lexer(std::string source);
    Token nextToken();
    std::vector<Token> tokenize();

private:
    std::string src;
    size_t pos;
    int line;
    int column;

    char current();
    char peek();
    void advance();
    void skipWhitespace();
    void skipComment();
    Token readIdentifier();
    Token readNumber();
    Token readString();
};

}
