#ifndef NOTA_LEXER_HPP
#define NOTA_LEXER_HPP

#include "Token.hpp"
#include <string>
#include <vector>
#include <unordered_map>

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> scanTokens();

private:
    const std::string& source;
    std::vector<Token> tokens;
    size_t start = 0;
    size_t current = 0;
    int line = 1;

    static const std::unordered_map<std::string, TokenType> keywords;

    bool isAtEnd();
    void scanToken();
    char advance();
    void addToken(TokenType type);
    char peek();

    void identifier();
    void string();
    void number();

    bool isDigit(char c);
    bool isAlpha(char c);
    bool isAlphaNumeric(char c);
};

#endif // NOTA_LEXER_HPP
