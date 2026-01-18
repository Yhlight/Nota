#include "lexer.h"
#include <cctype>
#include <iostream>

namespace nota {

Lexer::Lexer(std::string source) : src(std::move(source)), pos(0), line(1), column(1) {}

char Lexer::current() {
    if (pos >= src.length()) return '\0';
    return src[pos];
}

char Lexer::peek() {
    if (pos + 1 >= src.length()) return '\0';
    return src[pos + 1];
}

void Lexer::advance() {
    if (current() == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    pos++;
}

void Lexer::skipWhitespace() {
    while (std::isspace(current())) {
        advance();
    }
}

void Lexer::skipComment() {
    if (current() == '/' && peek() == '/') {
        while (current() != '\n' && current() != '\0') {
            advance();
        }
    } else if (current() == '/' && peek() == '*') {
        advance(); advance();
        while (!(current() == '*' && peek() == '/') && current() != '\0') {
            advance();
        }
        if (current() == '*') advance();
        if (current() == '/') advance();
    }
}

Token Lexer::nextToken() {
    skipWhitespace();

    while (current() == '/') {
        if (peek() == '/' || peek() == '*') {
            skipComment();
            skipWhitespace();
        } else {
            break;
        }
    }

    if (pos >= src.length()) {
        return {TokenType::EndOfFile, "", line, column};
    }

    if (std::isalpha(current()) || current() == '_') {
        return readIdentifier();
    }

    if (std::isdigit(current())) {
        return readNumber();
    }

    if (current() == '"') {
        return readString();
    }

    if (current() == '#') {
        int startCol = column;
        std::string text;
        text += current();
        advance();
        while (std::isxdigit(current())) {
            text += current();
            advance();
        }
        // Treat hex color as a string literal for simplicity in AST
        return {TokenType::String, text, line, startCol};
    }

    if (current() == ':') {
        Token t = {TokenType::Colon, ":", line, column};
        advance();
        return t;
    }

    if (current() == '{') {
        Token t = {TokenType::LBrace, "{", line, column};
        advance();
        return t;
    }

    if (current() == '}') {
        Token t = {TokenType::RBrace, "}", line, column};
        advance();
        return t;
    }

    if (current() == '%') {
        Token t = {TokenType::Percent, "%", line, column};
        advance();
        return t;
    }

    if (current() == ';') {
        Token t = {TokenType::Semicolon, ";", line, column};
        advance();
        return t;
    }

    if (current() == '+') { Token t = {TokenType::Plus, "+", line, column}; advance(); return t; }
    if (current() == '-') { Token t = {TokenType::Minus, "-", line, column}; advance(); return t; }
    if (current() == '*') { Token t = {TokenType::Star, "*", line, column}; advance(); return t; }
    if (current() == '/') { Token t = {TokenType::Slash, "/", line, column}; advance(); return t; }
    if (current() == '.') { Token t = {TokenType::Dot, ".", line, column}; advance(); return t; }
    if (current() == '(') { Token t = {TokenType::LParen, "(", line, column}; advance(); return t; }
    if (current() == ')') { Token t = {TokenType::RParen, ")", line, column}; advance(); return t; }

    Token t = {TokenType::Unknown, std::string(1, current()), line, column};
    advance();
    return t;
}

Token Lexer::readIdentifier() {
    int startCol = column;
    std::string text;
    while (std::isalnum(current()) || current() == '_') {
        text += current();
        advance();
    }
    return {TokenType::Identifier, text, line, startCol};
}

Token Lexer::readNumber() {
    int startCol = column;
    std::string text;
    while (std::isdigit(current())) {
        text += current();
        advance();
    }
    if (current() == '.') {
        text += current();
        advance();
        while (std::isdigit(current())) {
            text += current();
            advance();
        }
    }
    return {TokenType::Number, text, line, startCol};
}

Token Lexer::readString() {
    int startCol = column;
    advance(); // skip "
    std::string text;
    while (current() != '"' && current() != '\0') {
        text += current();
        advance();
    }
    if (current() == '"') advance();
    return {TokenType::String, text, line, startCol};
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (true) {
        Token t = nextToken();
        tokens.push_back(t);
        if (t.type == TokenType::EndOfFile) break;
    }
    return tokens;
}

}
