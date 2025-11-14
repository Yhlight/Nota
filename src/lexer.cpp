#include "lexer.h"
#include <cctype>

Lexer::Lexer(const std::string& input) : input(input), position(0), readPosition(0), ch(' ') {
    advance(); // Initialize ch
}

Token Lexer::nextToken() {
    skipWhitespace();

    switch (ch) {
        case '=':
            advance();
            return makeToken(TokenType::ASSIGN, "=");
        case '"':
            return string();
        case '\0':
            return makeToken(TokenType::END_OF_FILE, "");
        default:
            if (isalpha(ch) || ch == '_') {
                return identifier();
            } else if (isdigit(ch)) {
                return number();
            } else {
                Token token = makeToken(TokenType::ILLEGAL, std::string(1, ch));
                advance();
                return token;
            }
    }
}

void Lexer::skipWhitespace() {
    while (isspace(ch)) {
        advance();
    }
}

char Lexer::peek() {
    if (readPosition >= input.length()) {
        return '\0';
    } else {
        return input[readPosition];
    }
}

char Lexer::advance() {
    if (readPosition >= input.length()) {
        ch = '\0';
    } else {
        ch = input[readPosition];
    }
    position = readPosition;
    readPosition++;
    return ch;
}

Token Lexer::makeToken(TokenType type, const std::string& literal) {
    return Token{type, literal};
}

Token Lexer::identifier() {
    size_t startPosition = position;
    while (isalpha(ch) || isdigit(ch) || ch == '_') {
        advance();
    }
    std::string literal = input.substr(startPosition, position - startPosition);
    if (literal == "let") {
        return makeToken(TokenType::LET, literal);
    } else if (literal == "mut") {
        return makeToken(TokenType::MUT, literal);
    }
    return makeToken(TokenType::IDENTIFIER, literal);
}

Token Lexer::number() {
    size_t startPosition = position;
    bool isFloat = false;
    while (isdigit(ch) || ch == '.') {
        if (ch == '.') {
            isFloat = true;
        }
        advance();
    }
    std::string literal = input.substr(startPosition, position - startPosition);
    return makeToken(isFloat ? TokenType::FLOAT : TokenType::INTEGER, literal);
}

Token Lexer::string() {
    advance(); // Consume the opening "
    size_t startPosition = position;
    while (ch != '"' && ch != '\0') {
        advance();
    }
    std::string literal = input.substr(startPosition, position - startPosition);
    advance(); // Consume the closing "
    return makeToken(TokenType::STRING, literal);
}
