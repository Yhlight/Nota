#include "Lexer.hpp"

Lexer::Lexer(const std::string& source) : source(source) {}

#include <map>
#include <cctype>

std::map<std::string, TokenType> keywords = {
    {"App", TokenType::APP},
    {"Row", TokenType::ROW},
    {"Col", TokenType::COL},
    {"Rect", TokenType::RECT},
    {"Text", TokenType::TEXT},
    {"Item", TokenType::ITEM},
    {"parent", TokenType::PARENT},
    {"this", TokenType::THIS}
};

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }

    tokens.push_back({TokenType::END_OF_FILE, "", line});
    return tokens;
}

void Lexer::scanToken() {
    char c = advance();
    switch (c) {
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;
        case '[': addToken(TokenType::LEFT_BRACKET); break;
        case ']': addToken(TokenType::RIGHT_BRACKET); break;
        case ':': addToken(TokenType::COLON); break;
        case '.': addToken(TokenType::DOT); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case '*': addToken(TokenType::STAR); break;
        case '=': addToken(TokenType::EQUAL); break;
        case '%': addToken(TokenType::PERCENT); break;
        case '#': addToken(TokenType::HASH); break;
        case '/':
            if (match('/')) {
                // A comment goes until the end of the line.
                while (peek() != '\n' && !isAtEnd()) advance();
            } else if (match('*')) {
                // Block comment
                while (!isAtEnd() && !(peek() == '*' && peekNext() == '/')) {
                    if (peek() == '\n') line++;
                    advance();
                }
                if (!isAtEnd()) {
                    advance(); // consume *
                    advance(); // consume /
                }
            }
            else {
                addToken(TokenType::SLASH);
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
            if (isdigit(c)) {
                number();
            } else if (isalpha(c) || c == '_') {
                identifier();
            } else {
                // For now, we'll just ignore unknown characters.
                // A more robust implementation would report an error.
            }
            break;
    }
}

void Lexer::identifier() {
    while (isalnum(peek()) || peek() == '_') advance();

    std::string text = source.substr(start, current - start);
    TokenType type = keywords.count(text) ? keywords[text] : TokenType::IDENTIFIER;
    addToken(type);
}

void Lexer::number() {
    while (isdigit(peek())) advance();

    if (peek() == '.' && isdigit(peekNext())) {
        advance();
        while (isdigit(peek())) advance();
    }

    addToken(TokenType::NUMBER);
}

void Lexer::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }

    if (isAtEnd()) {
        // Unterminated string.
        // A more robust implementation would report an error.
        return;
    }

    advance(); // The closing ".

    // The lexeme will contain the quotes.
    addToken(TokenType::STRING);
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;

    current++;
    return true;
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
