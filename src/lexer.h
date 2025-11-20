#pragma once

#include <string>
#include <vector>

namespace nota {

enum class TokenType {
    // ... to be defined
};

struct Token {
    TokenType type;
    std::string value;
};

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    std::string source_;
    size_t current_ = 0;
};

} // namespace nota
