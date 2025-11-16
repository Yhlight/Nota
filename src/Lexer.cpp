#include "Lexer.h"
#include <cctype>

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (position < source.length()) {
        if (source[position] == '/' && position + 1 < source.length() && source[position + 1] == '/') {
            // It's a single-line comment, skip to the end of the line
            while (position < source.length() && source[position] != '\n') {
                position++;
            }
        } else if (source[position] == '/' && position + 1 < source.length() && source[position + 1] == '*') {
            // It's a multi-line comment
            position += 2; // Move past "/*"
            while (position + 1 < source.length() && (source[position] != '*' || source[position + 1] != '/')) {
                position++;
            }
            if (position + 1 < source.length()) {
                position += 2; // Move past "*/"
            } else {
                // Unterminated comment, stop lexing
                position = source.length();
            }
        } else if (isspace(source[position])) {
            // Skip whitespace
            position++;
        } else {
            // For now, just advance the position and add an "Unknown" token
            tokens.push_back({TokenType::Unknown, std::string(1, source[position])});
            position++;
        }
    }
    return tokens;
}
