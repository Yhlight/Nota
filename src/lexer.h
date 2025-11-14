#ifndef NOTA_LEXER_H
#define NOTA_LEXER_H

#include "token.h"
#include <string>

class Lexer {
public:
    Lexer(const std::string& input);
    Token nextToken();

private:
    void skipWhitespace();
    char peek();
    char advance();
    Token makeToken(TokenType type, const std::string& literal);
    Token identifier();
    Token number();
    Token string();

    std::string input;
    size_t position;
    size_t readPosition;
    char ch;
};

#endif //NOTA_LEXER_H
