#pragma once

#include "SourceLocation.h"
#include <string>

enum class TokenType {
    // Punctuators
    PLUS,           // +
    MINUS,          // -
    STAR,           // *
    SLASH,          // /
    PERCENT,        // %
    EQ,             // ==
    NEQ,            // !=
    GT,             // >
    GTE,            // >=
    LT,             // <
    LTE,            // <=
    NOT,            // !
    AMP,            // &
    PIPE,           // |
    CARET,          // ^
    TILDE,          // ~
    LSHIFT,         // <<
    RSHIFT,         // >>
    ASSIGN,         // =
    PLUS_ASSIGN,    // +=
    MINUS_ASSIGN,   // -=
    STAR_ASSIGN,    // *=
    SLASH_ASSIGN,   // /=
    PERCENT_ASSIGN, // %=
    AND,            // &&
    OR,             // ||
    INC,            // ++
    DEC,            // --
    LPAREN,         // (
    RPAREN,         // )
    LBRACE,         // {
    RBRACE,         // }
    LBRACKET,       // [
    RBRACKET,       // ]
    COMMA,          // ,
    DOT,            // .
    COLON,          // :
    SEMICOLON,      // ;
    ARROW,          // =>

    // Keywords
    LET,            // let
    MUT,            // mut
    IF,             // if
    ELSE,           // else
    END,            // end
    MATCH,          // match
    WHILE,          // while
    FOR,            // for
    DO,             // do
    FUNC,           // func
    CLASS,          // class
    RETURN,         // return
    IMPORT,         // import
    PACKAGE,        // package
    AS,             // as
    NONE,           // none
    PRINT,          // print

    // Literals
    IDENTIFIER,
    INTEGER,
    FLOAT,
    STRING,
    BOOL,

    // Misc
    NEWLINE,
    END_OF_FILE,
    UNRECOGNIZED
};

struct Token {
    TokenType type;
    std::string lexeme;
    SourceLocation location;
};
