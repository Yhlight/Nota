#ifndef LEXER_H
#define LEXER_H

#include "Token.h"
#include <string>
#include <vector>
#include <variant>

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> scanTokens();

private:
    std::string source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;

    bool isAtEnd();
    bool isDigit(char c);
    bool isAlpha(char c);
    bool isAlphaNumeric(char c);
    void scanToken();
    void string();
    void number();
    void identifier();
    char advance();
    void addToken(TokenType type);
    void addToken(TokenType type, const std::variant<std::monostate, std::string, double>& literal);
    bool match(char expected);
    char peek();
    char peekNext();
};

#endif // LEXER_H
