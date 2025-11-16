#include "Lexer.h"
#include <cctype>
#include <map>

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    std::map<std::string, TokenType> keywords = {
        {"let", TokenType::Let},
        {"mut", TokenType::Mut},
    };

    while (position < source.length()) {
        char current_char = source[position];

        if (isspace(current_char)) {
            position++;
            continue;
        }

        if (current_char == '/' && position + 1 < source.length()) {
            if (source[position + 1] == '/') {
                while (position < source.length() && source[position] != '\n') {
                    position++;
                }
                continue;
            } else if (source[position + 1] == '*') {
                position += 2;
                while (position + 1 < source.length() && (source[position] != '*' || source[position + 1] != '/')) {
                    position++;
                }
                if (position + 1 < source.length()) {
                    position += 2;
                } else {
                    position = source.length();
                }
                continue;
            }
        }

        if (isalpha(current_char) || current_char == '_') {
            size_t start = position;
            while (position < source.length() && (isalnum(source[position]) || source[position] == '_')) {
                position++;
            }
            std::string text = source.substr(start, position - start);
            auto it = keywords.find(text);
            if (it != keywords.end()) {
                tokens.push_back({it->second, text});
            } else {
                tokens.push_back({TokenType::Identifier, text});
            }
            continue;
        }

        if (isdigit(current_char)) {
            size_t start = position;
            bool has_decimal = false;
            while (position < source.length()) {
                if (source[position] == '.') {
                    if (has_decimal) {
                        break;
                    }
                    has_decimal = true;
                } else if (!isdigit(source[position])) {
                    break;
                }
                position++;
            }
            std::string text = source.substr(start, position - start);
            tokens.push_back({TokenType::Number, text});
            continue;
        }

        switch (current_char) {
            case '+': tokens.push_back({TokenType::Plus, "+"}); break;
            case '-': tokens.push_back({TokenType::Minus, "-"}); break;
            case '*': tokens.push_back({TokenType::Asterisk, "*"}); break;
            case '/': tokens.push_back({TokenType::Slash, "/"}); break;
            case '%': tokens.push_back({TokenType::Percent, "%"}); break;
            case '=': tokens.push_back({TokenType::Equal, "="}); break;
            default: tokens.push_back({TokenType::Unknown, std::string(1, current_char)}); break;
        }
        position++;
    }
    return tokens;
}
