#include "Lexer.h"
#include <map>

namespace nota {

    namespace {
        // Map of keywords to their token types
        const std::map<std::string, TokenType> keywords = {
            {"App", TokenType::APP},
            {"Row", TokenType::ROW},
            {"Col", TokenType::COL},
            // ... other keywords can be added here
        };
    }

    Lexer::Lexer(std::string source) : source(std::move(source)) {}

    bool Lexer::is_at_end() {
        return current >= source.length();
    }

    char Lexer::advance() {
        return source[current++];
    }

    void Lexer::add_token(TokenType type) {
        add_token(type, "");
    }

    void Lexer::add_token(TokenType type, const std::string& literal) {
        std::string text = source.substr(start, current - start);
        tokens.emplace_back(type, text, line);
    }

    void Lexer::scan_token() {
        char c = advance();
        switch (c) {
            case '{': add_token(TokenType::LEFT_BRACE); break;
            case '}': add_token(TokenType::RIGHT_BRACE); break;
            case ' ':
            case '\r':
            case '\t':
                // Ignore whitespace.
                break;
            case '\n':
                line++;
                break;
            default:
                if (isalpha(c)) {
                    while (isalnum(peek())) advance();
                    std::string text = source.substr(start, current - start);
                    auto it = keywords.find(text);
                    if (it != keywords.end()) {
                        add_token(it->second);
                    } else {
                        add_token(TokenType::IDENTIFIER);
                    }
                } else {
                    // For now, we'll just ignore unknown characters
                }
                break;
        }
    }

    char Lexer::peek() {
        if (is_at_end()) return '\0';
        return source[current];
    }


    std::vector<Token> Lexer::scan_tokens() {
        while (!is_at_end()) {
            start = current;
            scan_token();
        }

        tokens.emplace_back(TokenType::END_OF_FILE, "", line);
        return tokens;
    }

} // namespace nota
