#pragma once

#include "Token.hpp"
#include <string>
#include <vector>

class Lexer {
public:
    explicit Lexer(std::string source);
    std::vector<Token> scanTokens();

private:
    void scanToken();
    bool isAtEnd();
    char advance();
    char peek();
    void addToken(TokenType type);
    void addToken(TokenType type, const std::string& literal);

    std::string m_source;
    std::vector<Token> m_tokens;
    int m_start = 0;
    int m_current = 0;
    int m_line = 1;
};
