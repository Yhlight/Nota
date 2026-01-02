#include "Lexer.h"
#include <cctype>

Lexer::Lexer(std::string_view source)
    : source_(source), start_(source.data()), current_(source.data()), line_(1), column_(1) {}

void Lexer::skip_whitespace() {
    while (true) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                line_++;
                column_ = 0;
                advance();
                break;
            default:
                return;
        }
    }
}

Token Lexer::next_token() {
    while (true) {
        skip_whitespace();
        start_ = current_;

        if (is_at_end()) {
            return make_token(TokenType::END_OF_FILE);
        }

        if (peek() == '/' && peek_next() == '/') {
            while (peek() != '\n' && !is_at_end()) {
                advance();
            }
            continue;
        }

        if (peek() == '/' && peek_next() == '*') {
            advance();
            advance();
            while (!(peek() == '*' && peek_next() == '/') && !is_at_end()) {
                if (peek() == '\n') {
                    line_++;
                    column_ = 0;
                }
                advance();
            }
            if (is_at_end() || peek_next() == '\0') {
                return error_token("Unterminated comment.");
            }
            advance();
            advance();
            continue;
        }

        break;
    }

    char c = advance();

    if (std::isalpha(c) || c == '_') {
        return identifier();
    }
    if (std::isdigit(c)) {
        return number();
    }

    switch (c) {
        case '{': return make_token(TokenType::LEFT_BRACE);
        case '}': return make_token(TokenType::RIGHT_BRACE);
        case ':': return make_token(TokenType::COLON);
        case ';': return make_token(TokenType::SEMICOLON);
        case '"': return string();
        case '%': return make_token(TokenType::PERCENT);
        case '*': return make_token(TokenType::STAR);
        case '/': return make_token(TokenType::SLASH);
    }

    return error_token("Unexpected character.");
}

char Lexer::advance() {
    current_++;
    column_++;
    return current_[-1];
}

char Lexer::peek() const {
    return *current_;
}

char Lexer::peek_next() const {
    if (is_at_end()) return '\0';
    return current_[1];
}

bool Lexer::is_at_end() const {
    return *current_ == '\0';
}

Token Lexer::make_token(TokenType type) const {
    return Token{type, {start_, (size_t)(current_ - start_)}, line_, column_ - (int)(current_ - start_)};
}

Token Lexer::error_token(const char* message) const {
    return Token{TokenType::UNEXPECTED, message, line_, column_};
}

Token Lexer::string() {
    while (peek() != '"' && !is_at_end()) {
        if (peek() == '\n') {
            line_++;
            column_ = 0;
        }
        advance();
    }

    if (is_at_end()) {
        return error_token("Unterminated string.");
    }

    advance();
    return make_token(TokenType::STRING);
}

Token Lexer::number() {
    while (std::isdigit(peek())) {
        advance();
    }

    if (peek() == '.' && std::isdigit(peek_next())) {
        advance();
        while (std::isdigit(peek())) {
            advance();
        }
    }

    return make_token(TokenType::NUMBER);
}

Token Lexer::identifier() {
    while (std::isalnum(peek()) || peek() == '_') {
        advance();
    }

    std::string_view text(start_, current_ - start_);
    if (text == "Item") return make_token(TokenType::ITEM);

    return make_token(TokenType::IDENTIFIER);
}
