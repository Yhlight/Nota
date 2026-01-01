#pragma once

#include "Token.h"
#include <string>
#include <vector>

namespace nota {

class Lexer {
public:
    explicit Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    Token nextToken();
    void skipWhitespace();
    Token makeToken(TokenType type);
    Token identifier();
    Token number();

    char peek();
    char advance();
    [[nodiscard]] bool isAtEnd() const;

    std::string source_;
    size_t start_ = 0;
    size_t current_ = 0;
};

} // namespace nota
