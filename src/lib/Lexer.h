#ifndef NOTA_LEXER_H
#define NOTA_LEXER_H

#include <string>
#include <vector>
#include "Token.h"

namespace nota {

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> scanTokens();

private:
    char advance();
    void addToken(TokenType type);
    bool match(char expected);
    void scanToken();
    void string();
    void number();
    void identifier();

    const std::string source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;
};

} // namespace nota

#endif // NOTA_LEXER_H
