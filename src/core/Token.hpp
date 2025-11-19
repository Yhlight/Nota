#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

namespace nota {
namespace core {

enum class TokenType {
    // Special
    ILLEGAL,
    END_OF_FILE,
    NEWLINE,

    // Identifiers + literals
    IDENTIFIER,
    INTEGER,
    FLOAT,
    STRING,

    // Operators
    PLUS,           // +
    MINUS,          // -
    ASTERISK,       // *
    SLASH,          // /
    PERCENT,        // %
    ASSIGN,         // =
    BANG,           // !
    GT,             // >
    LT,             // <
    AMPERSAND,      // &
    PIPE,           // |
    CARET,          // ^
    TILDE,          // ~
    COLON,          // :
    DOT,            // .

    // Two-character operators
    PLUS_PLUS,      // ++
    MINUS_MINUS,    // --
    EQ,             // ==
    NOT_EQ,         // !=
    GT_EQ,          // >=
    LT_EQ,          // <=
    PLUS_ASSIGN,    // +=
    MINUS_ASSIGN,   // -=
    ASTERISK_ASSIGN,// *=
    SLASH_ASSIGN,   // /=
    PERCENT_ASSIGN, // %=
    AND,            // &&
    OR,             // ||
    ARROW,          // =>
    DOUBLE_COLON,   // ::

    // Delimiters
    LPAREN,         // (
    RPAREN,         // )
    LBRACE,         // {
    RBRACE,         // }
    LBRACKET,       // [
    RBRACKET,       // ]
    COMMA,          // ,

    // Keywords
    LET,
    MUT,
    IF,
    ELSE,
    END,
    MATCH,
    WHILE,
    FOR,
    DO,
    FUNC,
    CLASS,
    IMPORT,
    AS,
    PACKAGE,
    RETURN,
    THIS,
    NONE,
    TRUE,
    FALSE,
};

struct Token {
    TokenType type;
    std::string_view literal;
    int line;
};

const std::unordered_map<std::string, TokenType> keywords = {
    {"let", TokenType::LET},
    {"mut", TokenType::MUT},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"end", TokenType::END},
    {"match", TokenType::MATCH},
    {"while", TokenType::WHILE},
    {"for", TokenType::FOR},
    {"do", TokenType::DO},
    {"func", TokenType::FUNC},
    {"class", TokenType::CLASS},
    {"import", TokenType::IMPORT},
    {"as", TokenType::AS},
    {"package", TokenType::PACKAGE},
    {"return", TokenType::RETURN},
    {"this", TokenType::THIS},
    {"none", TokenType::NONE},
    {"true", TokenType::TRUE},
    {"false", TokenType::FALSE},
};

} // namespace core
} // namespace nota
