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
    void scanToken();
    bool isAtEnd();
    char advance();
    void addToken(TokenType type);

    bool match(char expected);
    char peek();
    char peekNext();

    void string();
    void number();
    void identifier();

    const std::string& source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;
};

#endif // NOTA_LEXER_HPP
