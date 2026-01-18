#ifndef NOTA_LEXER_HPP
#define NOTA_LEXER_HPP

#include "Token.hpp"
#include <string>
#include <vector>

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> scanTokens();

private:
    const std::string source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;

    bool isAtEnd();
    void scanToken();
    char advance();
    void addToken(TokenType type);
    char peek();
    char peekNext();
    void identifier();
    void number();
    void string();
};

#endif // NOTA_LEXER_HPP
