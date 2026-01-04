#include "Lexer.h"
#include "Token.h"
#include "TokenType.h"

#include <iostream>
#include <map>
#include <string>
#include <vector>

const std::map<std::string, TokenType> Lexer::keywords = {
    {"Item", TokenType::ITEM},
    {"import", TokenType::IMPORT},
    {"as", TokenType::AS},
    {"package", TokenType::PACKAGE},
    {"export", TokenType::EXPORT},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"true", TokenType::TRUE},
    {"false", TokenType::FALSE},
    {"onClick", TokenType::ON_CLICK},
    {"property", TokenType::PROPERTY},
};

Lexer::Lexer(std::string source) : source(std::move(source)) {}

std::vector<Token> Lexer::scan_tokens() {
    while (!is_at_end()) {
        start = current;
        scan_token();
    }

    tokens.emplace_back(TokenType::END_OF_FILE, "", nullptr, line);
    return tokens;
}

void Lexer::scan_token() {
    char c = advance();
    switch (c) {
        case '{': add_token(TokenType::LEFT_BRACE); break;
        case '}': add_token(TokenType::RIGHT_BRACE); break;
        case '(': add_token(TokenType::LEFT_PAREN); break;
        case ')': add_token(TokenType::RIGHT_PAREN); break;
        case '[': add_token(TokenType::LEFT_BRACKET); break;
        case ']': add_token(TokenType::RIGHT_BRACKET); break;
        case ':': add_token(TokenType::COLON); break;
        case ';': add_token(TokenType::SEMICOLON); break;
        case '.': add_token(TokenType::DOT); break;
        case ',': add_token(TokenType::COMMA); break;
        case '-': add_token(TokenType::MINUS); break;
        case '+': add_token(TokenType::PLUS); break;
        case '*': add_token(TokenType::STAR); break;
        case '%': add_token(TokenType::PERCENT); break;
        case '#': add_token(TokenType::HASH); break;
        case '/':
            if (match('/')) {
                // A comment goes until the end of the line.
                while (peek() != '\n' && !is_at_end()) advance();
            } else if (match('*')) {
                // A block comment goes until */
                while (!(peek() == '*' && peek_next() == '/') && !is_at_end()) {
                    if (peek() == '\n') line++;
                    advance();
                }
                if (!is_at_end()) {
                    advance(); // consume '*'
                    advance(); // consume '/'
                }
            }
            else {
                add_token(TokenType::SLASH);
            }
            break;

        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace.
            break;

        case '\n':
            line++;
            break;

        case '"': string(); break;

        default:
            if (is_digit(c)) {
                number();
            } else if (is_alpha(c)) {
                identifier();
            } else {
                // For now, just log an error message.
                // A more robust implementation would have an error reporting mechanism.
                 std::cerr << "Lexer Error: Unexpected character '" << c << "' at line " << line << std::endl;
            }
            break;
    }
}

char Lexer::advance() {
    return source[current++];
}

void Lexer::add_token(TokenType type) {
    add_token(type, nullptr);
}

void Lexer::add_token(TokenType type, const std::any& literal) {
    std::string text = source.substr(start, current - start);
    tokens.emplace_back(type, text, literal, line);
}

bool Lexer::match(char expected) {
    if (is_at_end()) return false;
    if (source[current] != expected) return false;
    current++;
    return true;
}

char Lexer::peek() {
    if (is_at_end()) return '\0';
    return source[current];
}

char Lexer::peek_next() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

void Lexer::string() {
    while (peek() != '"' && !is_at_end()) {
        if (peek() == '\n') line++;
        advance();
    }

    if (is_at_end()) {
        std::cerr << "Lexer Error: Unterminated string at line " << line << std::endl;
        return;
    }

    // The closing ".
    advance();

    // Trim the surrounding quotes.
    std::string value = source.substr(start + 1, current - start - 2);
    add_token(TokenType::STRING, value);
}

void Lexer::number() {
    while (is_digit(peek())) advance();

    // Look for a fractional part.
    if (peek() == '.' && is_digit(peek_next())) {
        // Consume the "."
        advance();

        while (is_digit(peek())) advance();
    }

    add_token(TokenType::NUMBER, std::stod(source.substr(start, current - start)));
}

void Lexer::identifier() {
    while (is_alpha_numeric(peek())) advance();

    std::string text = source.substr(start, current - start);
    TokenType type = TokenType::IDENTIFIER;
    if (keywords.count(text)) {
        type = keywords.at(text);
    }
    add_token(type);
}

bool Lexer::is_at_end() const {
    return current >= source.length();
}

bool Lexer::is_digit(char c) const {
    return c >= '0' && c <= '9';
}

bool Lexer::is_alpha(char c) const {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
            c == '_';
}

bool Lexer::is_alpha_numeric(char c) const {
    return is_alpha(c) || is_digit(c);
}
