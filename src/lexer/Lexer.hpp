#pragma once

#include "core/Token.hpp"
#include <string_view>
#include <vector>

namespace nota {
namespace lexer {

class Lexer {
public:
    explicit Lexer(std::string_view source);

    std::vector<core::Token> Tokenize();

private:
    core::Token NextToken();
    char Peek() const;
    char PeekNext() const;
    void Advance();
    void SkipWhitespace();
    void SkipComment();
    core::Token MakeToken(core::TokenType type) const;
    core::Token MakeIdentifier();
    core::Token MakeNumber();
    core::Token MakeString();

    std::string_view source_;
    size_t start_{0};
    size_t current_{0};
    int line_{1};
};

} // namespace lexer
} // namespace nota
