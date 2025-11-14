#pragma once

#include "token.hpp"
#include <string>
#include <vector>

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    Token next_token();
    char advance();
    char peek();
    char peek_next();
    bool is_at_end();
    bool match(char expected);
    void skip_whitespace();
    Token identifier();
    Token number();
    Token string();
    Token make_token(TokenType type);

    std::string source_;
    int start_ = 0;
    int current_ = 0;
    int line_ = 1;
};
