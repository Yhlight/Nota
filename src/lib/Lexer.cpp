#include "Lexer.h"
#include <unordered_map>

namespace nota {

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::scanTokens() {
    while (current < source.length()) {
        start = current;
        scanToken();
    }
    tokens.push_back({TokenType::END_OF_FILE, "", line});
    return tokens;
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
        case '-': addToken(TokenType::MINUS); break;
        case '+': addToken(TokenType::PLUS); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case '*': addToken(TokenType::STAR); break;
        case '!': addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG); break;
        case '=': addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
        case '<': addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
        case '>': addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;
        case '/':
            if (match('/')) {
                // A comment goes until the end of the line.
                while (current < source.length() && source[current] != '\n') {
                    advance();
                }
            } else {
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
                // Handle error
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

bool Lexer::match(char expected) {
    if (current >= source.length()) return false;
    if (source[current] != expected) return false;
    current++;
    return true;
}

void Lexer::string() {
    while (current < source.length() && source[current] != '"') {
        if (source[current] == '\n') line++;
        advance();
    }
    if (current >= source.length()) {
        // Unterminated string.
        return;
    }
    advance(); // The closing ".
    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING);
}

void Lexer::number() {
    while (isdigit(source[current])) advance();
    if (source[current] == '.' && isdigit(source[current + 1])) {
        advance();
        while (isdigit(source[current])) advance();
    }
    addToken(TokenType::NUMBER);
}

void Lexer::identifier() {
    while (isalnum(source[current]) || source[current] == '_') advance();
    std::string text = source.substr(start, current - start);
    TokenType type;

    static const std::unordered_map<std::string, TokenType> keywords = {
        {"and",    TokenType::AND},
        {"class",  TokenType::CLASS},
        {"else",   TokenType::ELSE},
        {"false",  TokenType::FALSE},
        {"for",    TokenType::FOR},
        {"fn",     TokenType::FN},
        {"if",     TokenType::IF},
        {"nil",    TokenType::NIL},
        {"or",     TokenType::OR},
        {"print",  TokenType::PRINT},
        {"return", TokenType::RETURN},
        {"super",  TokenType::SUPER},
        {"this",   TokenType::THIS},
        {"true",   TokenType::TRUE},
        {"let",    TokenType::LET},
        {"mut",    TokenType::MUT},
        {"while",  TokenType::WHILE},
    };

    auto it = keywords.find(text);
    if (it == keywords.end()) {
        type = TokenType::IDENTIFIER;
    } else {
        type = it->second;
    }

    addToken(type);
}

} // namespace nota
