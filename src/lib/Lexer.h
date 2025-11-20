#pragma once

#include "Token.h"
#include <string>
#include <vector>

namespace nota {

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> scanTokens();
    bool hadError() const { return hadError_; }

private:
    bool isAtEnd() const;
    void scanToken();
    char advance();
    void addToken(TokenType type);
    void addToken(TokenType type, const std::variant<std::monostate, int, double, std::string>& literal);
    bool match(char expected);
    char peek() const;
    void number();
    void identifier();
    void string();
    bool isDigit(char c) const;
    bool isAlpha(char c) const;
    bool isAlphaNumeric(char c) const;
    char peekNext() const;
    void report(int line, const std::string& where, const std::string& message);


    const std::string source_;
    std::vector<Token> tokens_;
    size_t start_ = 0;
    size_t current_ = 0;
    int line_ = 1;
    bool hadError_ = false;
};

} // namespace nota
