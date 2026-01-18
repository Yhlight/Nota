#include "Lexer.h"
#include <cctype>
#include <iostream>

Lexer::Lexer(const std::string& source) : source(source) {}

char Lexer::peek(int offset) const {
    if (position + offset >= source.length()) return '\0';
    return source[position + offset];
}

char Lexer::advance() {
    if (isAtEnd()) return '\0';
    column++;
    if (source[position] == '\n') {
        line++;
        column = 1;
    }
    return source[position++];
}

bool Lexer::isAtEnd() const {
    return position >= source.length();
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        if (isspace(c)) {
            advance();
        } else if (c == '/' && peek(1) == '/') {
            // Single line comment
            while (peek() != '\n' && !isAtEnd()) advance();
        } else if (c == '/' && peek(1) == '*') {
            // Multi-line comment
            advance(); advance();
            while (!isAtEnd()) {
                if (peek() == '*' && peek(1) == '/') {
                    advance(); advance();
                    break;
                }
                advance();
            }
        } else {
            break;
        }
    }
}

Token Lexer::readIdentifier() {
    int startLine = line;
    int startCol = column;
    std::string value;
    while (isalnum(peek()) || peek() == '_' || peek() == '%') { // % allowed for 100%
        value += advance();
    }
    return Token{TokenType::Identifier, value, startLine, startCol};
}

Token Lexer::readNumber() {
    int startLine = line;
    int startCol = column;
    std::string value;
    while (isdigit(peek()) || peek() == '.') {
         value += advance();
    }
    // Check if followed by %
    if (peek() == '%') {
         value += advance();
    }
    return Token{TokenType::Number, value, startLine, startCol};
}

Token Lexer::readString() {
    int startLine = line;
    int startCol = column;
    advance(); // Skip "
    std::string value;
    while (peek() != '"' && !isAtEnd()) {
        value += advance();
    }
    if (!isAtEnd()) advance(); // Skip closing "
    return Token{TokenType::String, value, startLine, startCol};
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (!isAtEnd()) {
        skipWhitespace();
        if (isAtEnd()) break;

        char c = peek();
        int startLine = line;
        int startCol = column;

        if (isalpha(c)) {
            tokens.push_back(readIdentifier());
        } else if (isdigit(c)) {
            tokens.push_back(readNumber());
        } else if (c == '"') {
            tokens.push_back(readString());
        } else if (c == '#') {
             // Handle color hex codes like #f0f0f0 as Identifiers for now, or strings
             std::string value;
             value += advance();
             while (isalnum(peek())) {
                 value += advance();
             }
             tokens.push_back(Token{TokenType::Identifier, value, startLine, startCol});
        } else {
            advance();
            switch (c) {
                case '{': tokens.push_back(Token{TokenType::LBrace, "{", startLine, startCol}); break;
                case '}': tokens.push_back(Token{TokenType::RBrace, "}", startLine, startCol}); break;
                case ':': tokens.push_back(Token{TokenType::Colon, ":", startLine, startCol}); break;
                case ';': tokens.push_back(Token{TokenType::Semicolon, ";", startLine, startCol}); break;
                default:
                    // Unknown char
                    break;
            }
        }
    }

    tokens.push_back(Token{TokenType::EndOfFile, "", line, column});
    return tokens;
}
