#include "Lexer.h"

namespace nota {

Lexer::Lexer(const std::string& source) : source_(source) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    Token token;
    do {
        token = nextToken();
        tokens.push_back(token);
    } while (token.type != TokenType::EndOfFile);
    return tokens;
}

// Helper function
bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
            c == '_';
}

bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

void Lexer::skipWhitespace() {
    while (true) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
            case '\n':
                advance();
                break;
            default:
                return;
        }
    }
}

Token Lexer::makeToken(TokenType type) {
    return {type, source_.substr(start_, current_ - start_)};
}

Token Lexer::identifier() {
    while (isAlpha(peek()) || isDigit(peek())) {
        advance();
    }
    return makeToken(TokenType::Identifier);
}

Token Lexer::number() {
    while (isDigit(peek())) {
        advance();
    }
    return makeToken(TokenType::Integer);
}

Token Lexer::nextToken() {
    skipWhitespace();
    start_ = current_;

    if (isAtEnd()) {
        return {TokenType::EndOfFile, ""};
    }

    char c = advance();

    if (isAlpha(c)) {
        return identifier();
    }

    if (isDigit(c)) {
        return number();
    }

    switch (c) {
        case '{': return makeToken(TokenType::LBrace);
        case '}': return makeToken(TokenType::RBrace);
        case ':': return makeToken(TokenType::Colon);
    }

    return {TokenType::Unknown, std::string(1, c)};
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source_[current_];
}

char Lexer::advance() {
    return source_[current_++];
}

bool Lexer::isAtEnd() const {
    return current_ >= source_.length();
}

} // namespace nota
