#include "Lexer.hpp"
#include <iostream>
#include <unordered_map>
#include <cctype>

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }

    tokens.push_back({TokenType::END_OF_FILE, "", line});
    return tokens;
}

bool Lexer::isAtEnd() {
    return current >= source.length();
}

void Lexer::scanToken() {
    char c = advance();
    switch (c) {
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;
        case ':': addToken(TokenType::COLON); break;
        case '(': addToken(TokenType::LEFT_PAREN); break;
        case ')': addToken(TokenType::RIGHT_PAREN); break;
        case '[': addToken(TokenType::LEFT_BRACKET); break;
        case ']': addToken(TokenType::RIGHT_BRACKET); break;
        case ',': addToken(TokenType::COMMA); break;
        case '.': addToken(TokenType::DOT); break;
        case '/':
            if (peek() == '/') {
                // A single-line comment goes until the end of the line.
                while (peek() != '\n' && !isAtEnd()) advance();
            } else if (peek() == '*') {
                // A multi-line comment goes until '*/'
                while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
                    if (peek() == '\n') line++;
                    advance();
                }
                if (!isAtEnd()) {
                    advance(); // consume '*'
                    advance(); // consume '/'
                }
            } else {
                addToken(TokenType::SLASH);
            }
            break;

        // Ignore whitespace
        case ' ':
        case '\r':
        case '\t':
            break;

        case '\n':
            line++;
            break;

        case '"': string(); break;
        case '%': addToken(TokenType::PERCENT); break;

        default:
            if (isdigit(c)) {
                number();
            } else if (isalpha(c)) {
                identifier();
            } else {
                addToken(TokenType::UNKNOWN);
            }
            break;
    }
}

char Lexer::advance() {
    return source[current++];
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source.at(current);
}

char Lexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

void Lexer::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }

    if (isAtEnd()) {
        addToken(TokenType::UNKNOWN); // Unterminated string.
        return;
    }

    // The closing ".
    advance();
    addToken(TokenType::STRING);
}

void Lexer::number() {
    while (isdigit(peek())) advance();

    // Look for a fractional part.
    if (peek() == '.' && isdigit(peekNext())) {
        advance(); // Consume the "."
        while (isdigit(peek())) advance();
    }

    addToken(TokenType::NUMBER);
}

void Lexer::identifier() {
    while (isalnum(peek()) || peek() == '_') {
        advance();
    }

    std::string text = source.substr(start, current - start);
    TokenType type;

    static const std::unordered_map<std::string, TokenType> keywords = {
        {"App", TokenType::APP},
        {"Row", TokenType::ROW},
        {"Col", TokenType::COL},
        {"Rect", TokenType::RECT},
        {"Text", TokenType::TEXT},
        {"Item", TokenType::ITEM},
        {"package", TokenType::PACKAGE},
        {"import", TokenType::IMPORT},
        {"export", TokenType::EXPORT},
        {"as", TokenType::AS},
        {"Button", TokenType::BUTTON},
        {"delegate", TokenType::DELEGATE},
        {"for", TokenType::FOR},
        {"states", TokenType::STATES},
        {"State", TokenType::STATE},
        {"when", TokenType::WHEN},
        {"if", TokenType::IF},
        {"else", TokenType::ELSE},
        {"property", TokenType::PROPERTY},
        {"Struct", TokenType::STRUCT},
        {"int", TokenType::TYPE_INT},
        {"double", TokenType::TYPE_DOUBLE},
        {"bool", TokenType::TYPE_BOOL},
        {"string", TokenType::TYPE_STRING},
        {"void", TokenType::TYPE_VOID},
        {"list", TokenType::TYPE_LIST},
        {"true", TokenType::TRUE},
        {"false", TokenType::FALSE}
    };

    auto it = keywords.find(text);
    if (it != keywords.end()) {
        type = it->second;
    } else {
        type = TokenType::IDENTIFIER;
    }
    addToken(type);
}

void Lexer::addToken(TokenType type) {
    std::string text = source.substr(start, current - start);
    tokens.push_back({type, text, line});
}
