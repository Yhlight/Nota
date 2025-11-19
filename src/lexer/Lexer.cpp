#include "Lexer.hpp"
#include <cctype>

namespace nota {
namespace lexer {

namespace {

bool is_alpha(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
}

bool is_digit(char c) {
    return std::isdigit(static_cast<unsigned char>(c));
}

bool is_alnum(char c) {
    return is_alpha(c) || is_digit(c);
}

} // namespace

Lexer::Lexer(std::string_view source) : source_(source) {}

std::vector<core::Token> Lexer::Tokenize() {
    std::vector<core::Token> tokens;
    core::Token token;
    do {
        token = NextToken();
        tokens.push_back(token);
    } while (token.type != core::TokenType::END_OF_FILE);
    return tokens;
}

core::Token Lexer::NextToken() {
    SkipWhitespace();
    start_ = current_;

    if (current_ >= source_.length()) {
        return MakeToken(core::TokenType::END_OF_FILE);
    }

    char c = source_[current_];
    Advance();

    if (is_alpha(c)) return MakeIdentifier();
    if (is_digit(c)) return MakeNumber();

    switch (c) {
        case '(': return MakeToken(core::TokenType::LPAREN);
        case ')': return MakeToken(core::TokenType::RPAREN);
        case '{': return MakeToken(core::TokenType::LBRACE);
        case '}': return MakeToken(core::TokenType::RBRACE);
        case '[': return MakeToken(core::TokenType::LBRACKET);
        case ']': return MakeToken(core::TokenType::RBRACKET);
        case ',': return MakeToken(core::TokenType::COMMA);
        case '.': return MakeToken(core::TokenType::DOT);
        case '~': return MakeToken(core::TokenType::TILDE);
        case '^': return MakeToken(core::TokenType::CARET);
        case '\n': return MakeToken(core::TokenType::NEWLINE);
        case '"': return MakeString();

        case '+':
            if (Peek() == '+') { Advance(); return MakeToken(core::TokenType::PLUS_PLUS); }
            if (Peek() == '=') { Advance(); return MakeToken(core::TokenType::PLUS_ASSIGN); }
            return MakeToken(core::TokenType::PLUS);
        case '-':
            if (Peek() == '-') { Advance(); return MakeToken(core::TokenType::MINUS_MINUS); }
            if (Peek() == '=') { Advance(); return MakeToken(core::TokenType::MINUS_ASSIGN); }
            return MakeToken(core::TokenType::MINUS);
        case '*':
            if (Peek() == '=') { Advance(); return MakeToken(core::TokenType::ASTERISK_ASSIGN); }
            return MakeToken(core::TokenType::ASTERISK);
        case '%':
            if (Peek() == '=') { Advance(); return MakeToken(core::TokenType::PERCENT_ASSIGN); }
            return MakeToken(core::TokenType::PERCENT);
        case '!':
            if (Peek() == '=') { Advance(); return MakeToken(core::TokenType::NOT_EQ); }
            return MakeToken(core::TokenType::BANG);
        case '=':
            if (Peek() == '=') { Advance(); return MakeToken(core::TokenType::EQ); }
            if (Peek() == '>') { Advance(); return MakeToken(core::TokenType::ARROW); }
            return MakeToken(core::TokenType::ASSIGN);
        case '>':
            if (Peek() == '=') { Advance(); return MakeToken(core::TokenType::GT_EQ); }
            return MakeToken(core::TokenType::GT);
        case '<':
            if (Peek() == '=') { Advance(); return MakeToken(core::TokenType::LT_EQ); }
            return MakeToken(core::TokenType::LT);
        case '&':
            if (Peek() == '&') { Advance(); return MakeToken(core::TokenType::AND); }
            return MakeToken(core::TokenType::AMPERSAND);
        case '|':
            if (Peek() == '|') { Advance(); return MakeToken(core::TokenType::OR); }
            return MakeToken(core::TokenType::PIPE);
        case ':':
            if (Peek() == ':') { Advance(); return MakeToken(core::TokenType::DOUBLE_COLON); }
            return MakeToken(core::TokenType::COLON);
        case '/':
            if (Peek() == '=') { Advance(); return MakeToken(core::TokenType::SLASH_ASSIGN); }
            return MakeToken(core::TokenType::SLASH);
    }

    return MakeToken(core::TokenType::ILLEGAL);
}

char Lexer::Peek() const {
    if (current_ >= source_.length()) return '\0';
    return source_[current_];
}

char Lexer::PeekNext() const {
    if (current_ + 1 >= source_.length()) return '\0';
    return source_[current_ + 1];
}

void Lexer::Advance() {
    current_++;
}

void Lexer::SkipWhitespace() {
    while (true) {
        char c = Peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                Advance();
                break;
            case '/':
                if (PeekNext() == '/') {
                    while (Peek() != '\n' && current_ < source_.length()) {
                        Advance();
                    }
                } else if (PeekNext() == '*') {
                    Advance(); // Consume '*'
                    Advance(); // Consume '/'
                    while (current_ < source_.length() && (Peek() != '*' || PeekNext() != '/')) {
                        if (Peek() == '\n') line_++;
                        Advance();
                    }
                    if (current_ < source_.length()) Advance(); // Consume '*'
                    if (current_ < source_.length()) Advance(); // Consume '/'
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

core::Token Lexer::MakeToken(core::TokenType type) const {
    return {type, source_.substr(start_, current_ - start_), line_};
}

core::Token Lexer::MakeIdentifier() {
    while (is_alnum(Peek())) {
        Advance();
    }
    auto literal = source_.substr(start_, current_ - start_);
    auto it = core::keywords.find(std::string(literal));
    if (it != core::keywords.end()) {
        return MakeToken(it->second);
    }
    return MakeToken(core::TokenType::IDENTIFIER);
}

core::Token Lexer::MakeNumber() {
    while (is_digit(Peek())) {
        Advance();
    }
    if (Peek() == '.' && is_digit(PeekNext())) {
        Advance();
        while (is_digit(Peek())) {
            Advance();
        }
        return MakeToken(core::TokenType::FLOAT);
    }
    return MakeToken(core::TokenType::INTEGER);
}

core::Token Lexer::MakeString() {
    while (Peek() != '"' && current_ < source_.length()) {
        if (Peek() == '\n') line_++;
        Advance();
    }

    if (current_ >= source_.length()) {
        return MakeToken(core::TokenType::ILLEGAL); // Unterminated string
    }

    Advance(); // The closing quote
    return MakeToken(core::TokenType::STRING);
}

} // namespace lexer
} // namespace nota
