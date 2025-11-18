#pragma once

#include "Token.h"
#include <string>
#include <vector>
#include <unordered_map>

class Lexer {
public:
    Lexer(std::string source);
    std::vector<Token> scanTokens();

private:
    const std::string source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;

    static const std::unordered_map<std::string, TokenType> keywords;

    bool isAtEnd();
    char advance();
    void addToken(TokenType type);
    void addToken(TokenType type, const std::variant<std::monostate, int, double, std::string>& literal);
    void scanToken();

    char peek();
    bool match(char expected);
    void number();
    void identifier();

    bool isDigit(char c);
    bool isAlpha(char c);
    bool isAlphaNumeric(char c);
};
