#pragma once

#include "Token.h"
#include <string>
#include <vector>
#include <map>

class Lexer {
public:
    explicit Lexer(std::string source);
    std::vector<Token> scan_tokens();

private:
    void scan_token();
    char advance();
    void add_token(TokenType type);
    void add_token(TokenType type, const std::any& literal);
    bool match(char expected);
    char peek();
    char peek_next();
    void string();
    void number();
    void identifier();

    bool is_at_end() const;
    bool is_digit(char c) const;
    bool is_alpha(char c) const;
    bool is_alpha_numeric(char c) const;

    const std::string source;
    std::vector<Token> tokens;
    static const std::map<std::string, TokenType> keywords;

    int start = 0;
    int current = 0;
    int line = 1;
};
