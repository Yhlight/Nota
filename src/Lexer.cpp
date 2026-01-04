#include "Lexer.h"
#include <map>

namespace nota {

    namespace {
        const std::map<std::string, TokenType> keywords = {
            {"App", TokenType::APP}, {"Row", TokenType::ROW}, {"Col", TokenType::COL},
            // ... other keywords
        };
    }

    Lexer::Lexer(std::string source) : source(std::move(source)) {}

    bool Lexer::is_at_end() { return current >= source.length(); }
    char Lexer::advance() { return source[current++]; }
    char Lexer::peek() { if (is_at_end()) return '\0'; return source[current]; }
    char Lexer::peek_next() { if (current + 1 >= source.length()) return '\0'; return source[current + 1]; }

    void Lexer::add_token(TokenType type) { add_token(type, ""); }
    void Lexer::add_token(TokenType type, const std::string& literal) {
        std::string text = source.substr(start, current - start);
        tokens.emplace_back(type, text, line);
    }

    void Lexer::string() {
        while (peek() != '"' && !is_at_end()) {
            if (peek() == '\n') line++;
            advance();
        }

        // Unterminated string.
        if (is_at_end()) {
            // Error handling will be added later
            return;
        }

        // The closing ".
        advance();

        // Trim the surrounding quotes and create the token.
        std::string value = source.substr(start + 1, current - start - 2);
        add_token(TokenType::STRING, value);
    }

    void Lexer::number() {
        while (isdigit(peek())) advance();
        if (peek() == '.' && isdigit(peek_next())) {
            advance();
            while (isdigit(peek())) advance();
        }
        add_token(TokenType::NUMBER, source.substr(start, current - start));
    }

    void Lexer::scan_token() {
        char c = advance();
        switch (c) {
            case '{': add_token(TokenType::LEFT_BRACE); break;
            case '}': add_token(TokenType::RIGHT_BRACE); break;
            case ':': add_token(TokenType::COLON); break;
            case '+': add_token(TokenType::PLUS); break;
            case '-': add_token(TokenType::MINUS); break;
            case '*': add_token(TokenType::STAR); break;
            case '/': add_token(TokenType::SLASH); break;
            case '"': string(); break;
            case ' ': case '\r': case '\t': break;
            case '\n': line++; break;
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
                } else if (isdigit(c)) {
                    number();
                } else {
                    // Handle unknown characters later
                }
                break;
        }
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
