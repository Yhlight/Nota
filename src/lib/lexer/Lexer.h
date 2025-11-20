#pragma once

#include <string>

namespace nota {

    enum class TokenType {
        EndOfFile,
        Unknown
    };

    struct Token {
        TokenType type;
        std::string value;
    };

    class Lexer {
    public:
        Lexer(const std::string& source);

        Token getNextToken();

    private:
        std::string source_;
        size_t position_;
    };

} // namespace nota
