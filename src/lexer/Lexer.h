#pragma once

#include "core/Token.h"
#include <string_view>

class Lexer {
public:
    Lexer(std::string_view source);

    Token next_token();

private:
    char advance();
    char peek() const;
    char peek_next() const;
    bool is_at_end() const;

    Token make_token(TokenType type) const;
    Token error_token(const char* message) const;
    Token string();
    Token number();
    Token identifier();

    void skip_whitespace();

    std::string_view source_;
    const char* start_;
    const char* current_;
    int line_;
    int column_;
};
