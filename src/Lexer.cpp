#include "Lexer.h"

Lexer::Lexer(const std::string& source) : source(source) {}

Token Lexer::nextToken() {
    if (current >= source.length()) {
        return {TokenType::Eof, "", line};
    }

    char c = source[current++];

    switch (c) {
        case '{': return {TokenType::LeftBrace, "{", line};
        default: return {TokenType::Unknown, std::string(1, c), line};
    }
}
