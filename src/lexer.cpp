#include "lexer.hpp"
#include <unordered_map>
#include <cctype>

static const std::unordered_map<std::string, TokenType> keywords = {
    {"let", TokenType::LET},
    {"mut", TokenType::MUT},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"end", TokenType::END},
    {"match", TokenType::MATCH},
    {"while", TokenType::WHILE},
    {"for", TokenType::FOR},
    {"do", TokenType::DO},
    {"func", TokenType::FUNC},
    {"return", TokenType::RETURN},
    {"class", TokenType::CLASS},
    {"new", TokenType::NEW},
    {"this", TokenType::THIS},
    {"import", TokenType::IMPORT},
    {"as", TokenType::AS},
    {"package", TokenType::PACKAGE},
    {"true", TokenType::TRUE},
    {"false", TokenType::FALSE},
};

Lexer::Lexer(const std::string& source) : source_(source) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!is_at_end()) {
        start_ = current_;
        Token token = next_token();
        if (token.type != TokenType::UNEXPECTED) {
            tokens.push_back(token);
        }
    }
    tokens.push_back({TokenType::END_OF_FILE, "", line_});
    return tokens;
}

Token Lexer::make_token(TokenType type) {
    return {type, source_.substr(start_, current_ - start_), line_};
}

bool Lexer::match(char expected) {
    if (is_at_end()) return false;
    if (source_[current_] != expected) return false;
    current_++;
    return true;
}

Token Lexer::next_token() {
    skip_whitespace();
    start_ = current_;

    if (is_at_end()) {
        return make_token(TokenType::END_OF_FILE);
    }

    char c = advance();

    if (std::isalpha(c) || c == '_') return identifier();
    if (std::isdigit(c)) return number();

    switch (c) {
        case '(': return make_token(TokenType::LPAREN);
        case ')': return make_token(TokenType::RPAREN);
        case '{': return make_token(TokenType::LBRACE);
        case '}': return make_token(TokenType::RBRACE);
        case '[': return make_token(TokenType::LBRACKET);
        case ']': return make_token(TokenType::RBRACKET);
        case ',': return make_token(TokenType::COMMA);
        case '.': return make_token(TokenType::DOT);
        case '+': return make_token(TokenType::PLUS);
        case '*': return make_token(TokenType::STAR);
        case '/': return make_token(TokenType::SLASH);
        case ';': return make_token(TokenType::SEMICOLON);
        case '"': return string();

        case '-':
            return make_token(TokenType::MINUS);
        case '=':
            return make_token(match('>') ? TokenType::ARROW : (match('=') ? TokenType::EQUAL : TokenType::ASSIGN));
        case '!':
            return make_token(match('=') ? TokenType::NOT_EQUAL : TokenType::UNEXPECTED);
        case '<':
            return make_token(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        case '>':
            return make_token(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
        case ':':
            return make_token(match(':') ? TokenType::COLON_COLON : TokenType::COLON);
    }

    return make_token(TokenType::UNEXPECTED);
}


char Lexer::advance() {
    return source_[current_++];
}

char Lexer::peek() {
    if (is_at_end()) return '\0';
    return source_[current_];
}

char Lexer::peek_next() {
    if (current_ + 1 >= source_.length()) return '\0';
    return source_[current_ + 1];
}

bool Lexer::is_at_end() {
    return current_ >= source_.length();
}

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
                advance();
                break;
            default:
                return;
        }
    }
}

Token Lexer::identifier() {
    while (std::isalnum(peek()) || peek() == '_') {
        advance();
    }

    std::string text = source_.substr(start_, current_ - start_);
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        return make_token(it->second);
    }

    return make_token(TokenType::IDENTIFIER);
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
        return make_token(TokenType::FLOAT);
    }

    return make_token(TokenType::INTEGER);
}

Token Lexer::string() {
    while (peek() != '"' && !is_at_end()) {
        if (peek() == '\n') line_++;
        advance();
    }

    if (is_at_end()) {
        // Unterminated string
        return make_token(TokenType::UNEXPECTED);
    }

    // The closing "
    advance();
    return make_token(TokenType::STRING);
}
