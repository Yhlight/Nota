#include "include/Lexer.hpp"

const std::unordered_map<std::string, TokenType> Lexer::keywords = {
    {"Item", TokenType::ITEM},
    {"App", TokenType::APP},
    {"Row", TokenType::ROW},
    {"Col", TokenType::COL},
    {"Rect", TokenType::RECT},
    {"Text", TokenType::TEXT},
    {"Button", TokenType::BUTTON},
    {"import", TokenType::IMPORT},
    {"as", TokenType::AS},
    {"package", TokenType::PACKAGE},
    {"export", TokenType::EXPORT},
    {"property", TokenType::PROPERTY},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"states", TokenType::STATES},
    {"State", TokenType::STATE},
    {"when", TokenType::WHEN},
    {"delegate", TokenType::DELEGATE},
    {"for", TokenType::FOR},
    {"Struct", TokenType::STRUCT}
};

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
        case '(': addToken(TokenType::LEFT_PAREN); break;
        case ')': addToken(TokenType::RIGHT_PAREN); break;
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;
        case ':': addToken(TokenType::COLON); break;
        case ';': addToken(TokenType::SEMICOLON); break;

        case '/':
            if (peek() == '/') {
                // A single line comment goes until the end of the line.
                while (peek() != '\n' && !isAtEnd()) advance();
            } else if (peek() == '*') {
                // A multi-line comment goes until */
                advance(); // consume the '*'
                while (!isAtEnd() && !(peek() == '*' && current + 1 < source.length() && source[current + 1] == '/')) {
                    if (peek() == '\n') line++;
                    advance();
                }
                if (!isAtEnd()) {
                    advance(); // consume the '*'
                    advance(); // consume the '/'
                }
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

        default:
            if (isAlpha(c)) {
                identifier();
            } else if (isDigit(c)) {
                number();
            } else {
                // For now, we'll just advance past unknown characters
                // In the future, we'll handle errors here
            }
            break;
    }
}

char Lexer::advance() {
    return source[current++];
}

void Lexer::addToken(TokenType type) {
    std::string text = source.substr(start, current - start);
    tokens.push_back({type, text, line});
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

void Lexer::identifier() {
    while (isAlphaNumeric(peek())) advance();

    std::string text = source.substr(start, current - start);
    TokenType type = TokenType::IDENTIFIER;
    if (keywords.count(text)) {
        type = keywords.at(text);
    }
    addToken(type);
}

bool Lexer::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
            c == '_';
}

bool Lexer::isAlphaNumeric(char c) {
    return isAlpha(c) || (c >= '0' && c <= '9');
}

bool Lexer::isDigit(char c) {
    return c >= '0' && c <= '9';
}

void Lexer::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }

    // Unterminated string
    if (isAtEnd()) {
        // Handle error in the future
        return;
    }

    // The closing ".
    advance();

    addToken(TokenType::STRING);
}

void Lexer::number() {
    while (isDigit(peek())) advance();

    // Look for a fractional part.
    if (peek() == '.' && isDigit(source[current + 1])) {
        // Consume the "."
        advance();

        while (isDigit(peek())) advance();
    }

    addToken(TokenType::NUMBER);
}
