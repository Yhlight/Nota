#include "lexer/Lexer.h"

const std::unordered_map<std::string, TokenType> Lexer::keywords = {
    {"let", TokenType::LET},
    {"mut", TokenType::MUT},
    {"func", TokenType::FUNC},
    {"class", TokenType::CLASS},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"while", TokenType::WHILE},
    {"for", TokenType::FOR},
    {"return", TokenType::RETURN},
    {"true", TokenType::TRUE},
    {"false", TokenType::FALSE},
};

Lexer::Lexer(std::string source) : source(std::move(source)) {}

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }

    tokens.push_back({TokenType::END_OF_FILE, "", std::monostate{}, line});
    return tokens;
}

bool Lexer::isAtEnd() {
    return current >= source.length();
}

char Lexer::advance() {
    return source[current++];
}

void Lexer::addToken(TokenType type) {
    addToken(type, std::monostate{});
}

void Lexer::addToken(TokenType type, const std::variant<std::monostate, int, double, std::string>& literal) {
    std::string text = source.substr(start, current - start);
    tokens.push_back({type, text, literal, line});
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;

    current++;
    return true;
}

void Lexer::scanToken() {
    char c = advance();
    switch (c) {
        case '(': addToken(TokenType::LPAREN); break;
        case ')': addToken(TokenType::RPAREN); break;
        case '{': addToken(TokenType::LBRACE); break;
        case '}': addToken(TokenType::RBRACE); break;
        case '+': addToken(TokenType::PLUS); break;
        case '-': addToken(TokenType::MINUS); break;
        case '*': addToken(TokenType::ASTERISK); break;
        case '/': addToken(TokenType::SLASH); break;
        case '=':
            addToken(match('=') ? TokenType::EQUALS : TokenType::ASSIGN);
            break;
        case '!':
            addToken(match('=') ? TokenType::NOT_EQUALS : TokenType::LOGICAL_NOT);
            break;
        case '<':
            addToken(match('=') ? TokenType::LESS_EQUALS : TokenType::LESS);
            break;
        case '>':
            addToken(match('=') ? TokenType::GREATER_EQUALS : TokenType::GREATER);
            break;

        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace.
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
                addToken(TokenType::ILLEGAL);
            }
            break;
    }
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

void Lexer::number() {
    while (isDigit(peek())) advance();

    // Look for a fractional part.
    if (peek() == '.' && isDigit(peekNext())) {
        // Consume the "."
        advance();

        while (isDigit(peek())) advance();
        addToken(TokenType::FLOAT, std::stod(source.substr(start, current - start)));
        return;
    }

    addToken(TokenType::INTEGER, std::stoi(source.substr(start, current - start)));
}

void Lexer::identifier() {
    while (isAlphaNumeric(peek())) advance();

    std::string text = source.substr(start, current - start);
    auto it = keywords.find(text);
    if (it == keywords.end()) {
        addToken(TokenType::IDENTIFIER);
    } else {
        addToken(it->second);
    }
}

bool Lexer::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool Lexer::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Lexer::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}
