#include "lexer.hpp"

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
    // Empty for now
}
