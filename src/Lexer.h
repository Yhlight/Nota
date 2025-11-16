#pragma once

#include <string>
#include <vector>

enum class TokenType {
    Comment,
    Unknown,
    // Placeholder
};

struct Token {
    TokenType type;
    std::string text;
};

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();
private:
    std::string source;
    size_t position = 0;
};
