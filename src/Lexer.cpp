#include "Lexer.h"
#include <cctype>
#include <iostream>
#include <map>

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
    while (isalnum(peek()) || peek() == '_' || peek() == '%') {
        value += advance();
    }

    // Check keywords
    static const std::map<std::string, TokenType> keywords = {
        {"if", TokenType::If},
        {"else", TokenType::Else},
        {"true", TokenType::True},
        {"false", TokenType::False},
        {"import", TokenType::Import},
        {"package", TokenType::Package},
        {"export", TokenType::Export},
        {"as", TokenType::As}
    };

    auto it = keywords.find(value);
    TokenType type = TokenType::Identifier;
    if (it != keywords.end()) {
        type = it->second;
    }

    return Token{type, value, startLine, startCol};
}

Token Lexer::readNumber() {
    int startLine = line;
    int startCol = column;
    std::string value;
    while (isdigit(peek()) || peek() == '.') {
         value += advance();
    }
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
                case '(': tokens.push_back(Token{TokenType::LParen, "(", startLine, startCol}); break;
                case ')': tokens.push_back(Token{TokenType::RParen, ")", startLine, startCol}); break;
                case ':': tokens.push_back(Token{TokenType::Colon, ":", startLine, startCol}); break;
                case ';': tokens.push_back(Token{TokenType::Semicolon, ";", startLine, startCol}); break;
                case '.': tokens.push_back(Token{TokenType::Dot, ".", startLine, startCol}); break;
                case ',': tokens.push_back(Token{TokenType::Comma, ",", startLine, startCol}); break;
                case '+': tokens.push_back(Token{TokenType::Plus, "+", startLine, startCol}); break;
                case '-': tokens.push_back(Token{TokenType::Minus, "-", startLine, startCol}); break;
                case '*': tokens.push_back(Token{TokenType::Star, "*", startLine, startCol}); break;
                case '/': tokens.push_back(Token{TokenType::Slash, "/", startLine, startCol}); break;
                case '!':
                    if (peek() == '=') {
                        advance();
                        tokens.push_back(Token{TokenType::BangEqual, "!=", startLine, startCol});
                    } else {
                        tokens.push_back(Token{TokenType::Bang, "!", startLine, startCol});
                    }
                    break;
                case '=':
                    if (peek() == '=') {
                        advance();
                        tokens.push_back(Token{TokenType::EqualEqual, "==", startLine, startCol});
                    } else {
                        tokens.push_back(Token{TokenType::Equal, "=", startLine, startCol});
                    }
                    break;
                case '<':
                    if (peek() == '=') {
                        advance();
                        tokens.push_back(Token{TokenType::LessEqual, "<=", startLine, startCol});
                    } else {
                        tokens.push_back(Token{TokenType::Less, "<", startLine, startCol});
                    }
                    break;
                case '>':
                    if (peek() == '=') {
                        advance();
                        tokens.push_back(Token{TokenType::GreaterEqual, ">=", startLine, startCol});
                    } else {
                        tokens.push_back(Token{TokenType::Greater, ">", startLine, startCol});
                    }
                    break;
                case '&':
                    if (peek() == '&') {
                        advance();
                        tokens.push_back(Token{TokenType::AmpAmp, "&&", startLine, startCol});
                    } else {
                        // Unknown single &
                         tokens.push_back(Token{TokenType::Unknown, "&", startLine, startCol});
                    }
                    break;
                case '|':
                    if (peek() == '|') {
                        advance();
                        tokens.push_back(Token{TokenType::PipePipe, "||", startLine, startCol});
                    } else {
                        // Unknown single |
                         tokens.push_back(Token{TokenType::Unknown, "|", startLine, startCol});
                    }
                    break;
                default:
                    tokens.push_back(Token{TokenType::Unknown, std::string(1, c), startLine, startCol});
                    break;
            }
        }
    }

    tokens.push_back(Token{TokenType::EndOfFile, "", line, column});
    return tokens;
}
