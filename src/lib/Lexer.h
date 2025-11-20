#pragma once

#include "Token.h"
#include <string>
#include <vector>

namespace nota {

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> scanTokens();

private:
    void scanToken();
    char advance();
    void addToken(TokenType type);
    void addToken(TokenType type, const std::string& literal);
    bool isAtEnd();
    bool match(char expected);
    char peek();
    char peekNext();
    void string();
    void number();
    void identifier();
    void skipWhitespace();

    const std::string& source_;
    std::vector<Token> tokens_;
    int start_ = 0;
    int current_ = 0;
    int line_ = 1;
    int column_ = 0;
};

} // namespace nota
