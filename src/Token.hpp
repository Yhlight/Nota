#ifndef NOTA_TOKEN_HPP
#define NOTA_TOKEN_HPP

#include <string>

namespace nota {

    enum class TokenType {
        // Single-character tokens
        PLUS, MINUS, STAR, SLASH,
        LEFT_PAREN, RIGHT_PAREN,

        // Literals
        NUMBER,
        IDENTIFIER,

        // Keywords

        // Other
        UNEXPECTED,
        END_OF_FILE
    };

    struct Token {
        TokenType type;
        std::string lexeme;
        int line;
        int column;
    };

} // namespace nota

#endif // NOTA_TOKEN_HPP
