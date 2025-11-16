#include "lexer.hpp"
#include "token.hpp"
#include <iostream>

Lexer::Lexer(const std::string& source) : source(source) {
    keywords["and"] = TokenType::AND;
    keywords["class"] = TokenType::CLASS;
    keywords["else"] = TokenType::ELSE;
    keywords["false"] = TokenType::FALSE;
    keywords["for"] = TokenType::FOR;
    keywords["fun"] = TokenType::FUN;
    keywords["if"] = TokenType::IF;
    keywords["nil"] = TokenType::NIL;
    keywords["or"] = TokenType::OR;
    keywords["print"] = TokenType::PRINT;
    keywords["return"] = TokenType::RETURN;
    keywords["super"] = TokenType::SUPER;
    keywords["this"] = TokenType::THIS;
    keywords["true"] = TokenType::TRUE;
    keywords["var"] = TokenType::VAR;
    keywords["while"] = TokenType::WHILE;
    keywords["let"] = TokenType::LET;
    keywords["mut"] = TokenType::MUT;
    keywords["end"] = TokenType::END;
    keywords["do"] = TokenType::DO;
}

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

char Lexer::advance() {
    return source[current++];
}

void Lexer::addToken(TokenType type) {
    std::string text = source.substr(start, current - start);
    tokens.push_back({type, text, line});
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    current++;
    return true;
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
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
        // Unterminated string
        std::cerr << "Unterminated string at line " << line << std::endl;
        return;
    }

    // The closing ".
    advance();

    // Trim the surrounding quotes
    std::string value = source.substr(start, current - start);
    addToken(TokenType::STRING);
}

void Lexer::number() {
    while (isDigit(peek())) advance();

    // Look for a fractional part
    if (peek() == '.' && isDigit(peekNext())) {
        // Consume the "."
        advance();

        while (isDigit(peek())) advance();
    }

    addToken(TokenType::NUMBER);
}

void Lexer::identifier() {
    while (isAlphaNumeric(peek())) advance();

    std::string text = source.substr(start, current - start);
    TokenType type = keywords.count(text) ? keywords[text] : TokenType::IDENTIFIER;
    addToken(type);
}

bool Lexer::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool Lexer::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
            c == '_';
}

bool Lexer::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

void Lexer::scanToken() {
    char c = advance();
    switch (c) {
        case '(': addToken(TokenType::LEFT_PAREN); break;
        case ')': addToken(TokenType::RIGHT_PAREN); break;
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;
        case ',': addToken(TokenType::COMMA); break;
        case '.': addToken(TokenType::DOT); break;
        case '-': addToken(TokenType::MINUS); break;
        case '+': addToken(TokenType::PLUS); break;
        case '*': addToken(TokenType::STAR); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case '/':
            if (match('/')) {
                // A comment goes until the end of the line.
                while (peek() != '\n' && !isAtEnd()) advance();
            } else {
                addToken(TokenType::SLASH);
            }
            break;
        case '!': addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG); break;
        case '=': addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
        case '<': addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
        case '>': addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;
        case '"': string(); break;
        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace
            break;
        case '\n':
            line++;
            break;
        default:
            if (isDigit(c)) {
                number();
            } else if (isAlpha(c)) {
                identifier();
            } else {
                // Error handling
                std::cerr << "Unexpected character at line " << line << ": " << c << std::endl;
            }
            break;
    }
}
