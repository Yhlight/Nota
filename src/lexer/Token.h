#pragma once

#include <string>

namespace nota {

    enum class TokenType {
        // Keywords
        App,
        Row,
        Col,
        Rect,
        Text,
        Item,

        // Symbols
        LBrace,     // {
        RBrace,     // }
        Colon,      // :
        Semicolon,  // ;

        // Literals
        Identifier,
        Integer,
        Percentage,
        HexColor,
        String,

        // Other
        EndOfFile,
        Unknown
    };

    struct Token {
        TokenType type;
        std::string text;
    };

} // namespace nota
