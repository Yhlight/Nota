#pragma once
#include <string>
#include <string_view>
#include <ostream>

enum class TokenType {
    // Basic
    Identifier,
    Number,
    String,

    // Symbols
    LBrace,     // {
    RBrace,     // }
    LParen,     // (
    RParen,     // )
    Colon,      // :
    Semicolon,  // ;
    Dot,        // .
    Comma,      // ,

    // Operators
    Plus,       // +
    Minus,      // -
    Star,       // *
    Slash,      // /
    Bang,       // !
    Equal,      // = (Assignment? or just for property?) currently property uses :
    EqualEqual, // ==
    BangEqual,  // !=
    Less,       // <
    LessEqual,  // <=
    Greater,    // >
    GreaterEqual, // >=
    AmpAmp,     // &&
    PipePipe,   // ||

    // Keywords
    If, Else, True, False,
    Import, Package, Export, As,

    EndOfFile,
    Unknown
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;

    friend std::ostream& operator<<(std::ostream& os, const Token& token) {
        os << "Token(" << static_cast<int>(token.type) << ", \"" << token.value << "\", " << token.line << ":" << token.column << ")";
        return os;
    }
};
