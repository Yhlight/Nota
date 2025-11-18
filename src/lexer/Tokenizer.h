#pragma once

#include "Token.h"
#include <string>
#include <vector>

class Tokenizer {
public:
    Tokenizer(const std::string& source);
    std::vector<Token> tokenize();

private:
    Token next_token();
    Token make_token(TokenType type, const std::string& lexeme);
    Token identifier();
    Token number();
    Token string();
    Token punctuator();

    char peek();
    char advance();
    bool is_at_end();
    void skip_whitespace_and_comments();

    std::string source_;
    size_t current_ = 0;
    SourceLocation location_;
};
