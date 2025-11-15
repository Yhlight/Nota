#pragma once

#include "token.hpp"
#include <string>
#include <vector>
#include <map>

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> scanTokens();

private:
    std::string source;
    std::vector<Token> tokens;
    std::map<std::string, TokenType> keywords;
    int start = 0;
    int current = 0;
    int line = 1;

    bool isAtEnd();
    char advance();
    void addToken(TokenType type);
    void scanToken();
    bool match(char expected);
    char peek();
    char peekNext();
    void string();
    void number();
    void identifier();
    bool isDigit(char c);
    bool isAlpha(char c);
    bool isAlphaNumeric(char c);
};
