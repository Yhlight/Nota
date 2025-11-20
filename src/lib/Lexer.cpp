#include "Lexer.h"
#include "Token.h"
#include <unordered_map>

namespace nota {

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
    {"fn", TokenType::FN},
    {"return", TokenType::RETURN},
    {"class", TokenType::CLASS},
    {"new", TokenType::NEW},
    {"this", TokenType::THIS},
    {"import", TokenType::IMPORT},
    {"as", TokenType::AS},
    {"package", TokenType::PACKAGE},
    {"none", TokenType::NONE},
    {"true", TokenType::TRUE},
    {"false", TokenType::FALSE},
};

Lexer::Lexer(const std::string& source) : source_(source) {}

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        start_ = current_;
        scanToken();
    }

    tokens_.push_back({TokenType::END_OF_FILE, "", line_, column_});
    return tokens_;
}

void Lexer::scanToken() {
    char c = advance();
    switch (c) {
        case '(': addToken(TokenType::LEFT_PAREN); break;
        case ')': addToken(TokenType::RIGHT_PAREN); break;
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;
        case '[': addToken(TokenType::LEFT_BRACKET); break;
        case ']': addToken(TokenType::RIGHT_BRACKET); break;
        case ',': addToken(TokenType::COMMA); break;
        case '.': addToken(TokenType::DOT); break;
        case '-':
            if (match('=')) {
                addToken(TokenType::MINUS_EQUAL);
            } else if (match('-')) {
                addToken(TokenType::MINUS_MINUS);
            } else {
                addToken(TokenType::MINUS);
            }
            break;
        case '+':
            if (match('=')) {
                addToken(TokenType::PLUS_EQUAL);
            } else if (match('+')) {
                addToken(TokenType::PLUS_PLUS);
            } else {
                addToken(TokenType::PLUS);
            }
            break;
        case '*':
            addToken(match('=') ? TokenType::STAR_EQUAL : TokenType::STAR);
            break;
        case '%':
            addToken(match('=') ? TokenType::MODULO_EQUAL : TokenType::MODULO);
            break;
        case ':': addToken(TokenType::COLON); break;
        case '!':
            addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
            break;
        case '=':
            addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
            break;
        case '<':
            addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
            break;
        case '>':
            addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
            break;
        case '/':
            if (match('/')) {
                // A comment goes until the end of the line.
                while (peek() != '\n' && !isAtEnd()) advance();
            } else if (match('*')) {
                // A multi-line comment goes until */
                while (peek() != '*' && peekNext() != '/' && !isAtEnd()) {
                    if (peek() == '\n') {
                        line_++;
                        column_ = 0;
                    }
                    advance();
                }
                if (!isAtEnd()) {
                    advance(); // consume the '*'
                    advance(); // consume the '/'
                }
            }
            else {
                addToken(match('=') ? TokenType::SLASH_EQUAL : TokenType::SLASH);
            }
            break;
        case '"': string(); break;
        case '\n':
            addToken(TokenType::NEWLINE);
            line_++;
            column_ = 0;
            break;
        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace.
            break;
        default:
            if (isdigit(c)) {
                number();
            } else if (isalpha(c) || c == '_') {
                identifier();
            } else {
                addToken(TokenType::UNKNOWN);
            }
            break;
    }
}

char Lexer::advance() {
    column_++;
    return source_[current_++];
}

void Lexer::addToken(TokenType type) {
    addToken(type, "");
}

void Lexer::addToken(TokenType type, const std::string& literal) {
    std::string text = source_.substr(start_, current_ - start_);
    tokens_.push_back({type, text, line_, column_});
}

bool Lexer::isAtEnd() {
    return current_ >= source_.length();
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source_[current_] != expected) return false;

    current_++;
    column_++;
    return true;
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source_[current_];
}

char Lexer::peekNext() {
    if (current_ + 1 >= source_.length()) return '\0';
    return source_[current_ + 1];
}

void Lexer::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') {
            line_++;
            column_ = 0;
        }
        advance();
    }

    if (isAtEnd()) {
        // Unterminated string.
        addToken(TokenType::UNKNOWN);
        return;
    }

    // The closing ".
    advance();

    // Trim the surrounding quotes.
    std::string value = source_.substr(start_ + 1, current_ - start_ - 2);
    addToken(TokenType::STRING, value);
}

void Lexer::number() {
    bool isFloat = false;
    while (isdigit(peek())) advance();

    // Look for a fractional part.
    if (peek() == '.' && isdigit(peekNext())) {
        isFloat = true;
        // Consume the "."
        advance();

        while (isdigit(peek())) advance();
    }

    addToken(isFloat ? TokenType::FLOAT : TokenType::INTEGER, source_.substr(start_, current_ - start_));
}

void Lexer::identifier() {
    while (isalnum(peek()) || peek() == '_') advance();

    std::string text = source_.substr(start_, current_ - start_);
    auto it = keywords.find(text);
    if (it == keywords.end()) {
        addToken(TokenType::IDENTIFIER);
    } else {
        addToken(it->second);
    }
}


} // namespace nota
