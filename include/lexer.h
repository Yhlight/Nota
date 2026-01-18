#pragma once

#include "token.h"
#include <string>
#include <vector>

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    std::string source;
    size_t pos;
    size_t len;
    int line;
    int column;

    char peek(int offset = 0) const;
    char advance();
    bool match(char expected);
    void skipWhitespace();

    Token makeToken(TokenType type, std::string value);

    Token readIdentifierOrKeyword();
    Token readNumber();
    Token readString();
    Token readHexColor();
    void skipComment();
};
