#pragma once

#include "lexer.h"
#include <vector>

namespace nota {

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    void parse();

private:
    std::vector<Token> tokens_;
    size_t current_ = 0;
};

} // namespace nota
