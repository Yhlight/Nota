#pragma once

#include "include/Token.hpp"
#include <string>

class Lexer {
public:
    Lexer(const std::string& source);
    Token nextToken();

private:
    std::string source;
    size_t current {0};
    int line {1};
};
