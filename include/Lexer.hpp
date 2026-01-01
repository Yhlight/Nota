#pragma once

#include "Token.hpp"
#include <string>
#include <vector>

class Lexer {
public:
    explicit Lexer(std::string source);
    std::vector<Token> scanTokens();

private:
    std::string m_source;
    std::vector<Token> m_tokens;
};
