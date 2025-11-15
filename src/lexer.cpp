#include "lexer.hpp"
#include <unordered_map>

namespace nota {

    static std::unordered_map<std::string, TokenType> keywords = {
        {"class",  TokenType::CLASS},
        {"else",   TokenType::ELSE},
        {"func",   TokenType::FUNC},
        {"for",    TokenType::FOR},
        {"if",     TokenType::IF},
        {"let",    TokenType::LET},
        {"mut",    TokenType::MUT},
        {"return", TokenType::RETURN},
        {"while",  TokenType::WHILE},
        {"end",    TokenType::END},
        {"match",  TokenType::MATCH},
        {"do",     TokenType::DO},
        {"import", TokenType::IMPORT},
        {"package",TokenType::PACKAGE},
        {"true",   TokenType::TRUE},
        {"false",  TokenType::FALSE}
    };

    Lexer::Lexer(const std::string& source) : source(source) {}

    std::vector<Token> Lexer::scan_tokens() {
        while (current < source.length()) {
            start = current;
            scan_token();
        }

        tokens.push_back({TokenType::EOF_TOKEN, "", line});
        return tokens;
    }

    void Lexer::scan_token() {
        char c = advance();
        switch (c) {
            case '(': add_token(TokenType::LEFT_PAREN); break;
            case ')': add_token(TokenType::RIGHT_PAREN); break;
            case '{': add_token(TokenType::LEFT_BRACE); break;
            case '}': add_token(TokenType::RIGHT_BRACE); break;
            case '[': add_token(TokenType::LEFT_BRACKET); break;
            case ']': add_token(TokenType::RIGHT_BRACKET); break;
            case ',': add_token(TokenType::COMMA); break;
            case '.': add_token(TokenType::DOT); break;
            case ';': add_token(TokenType::SEMICOLON); break;
            case '-':
                add_token(match('=') ? TokenType::MINUS_EQUAL : TokenType::MINUS);
                break;
            case '+':
                add_token(match('=') ? TokenType::PLUS_EQUAL : TokenType::PLUS);
                break;
            case '*':
                add_token(match('=') ? TokenType::STAR_EQUAL : TokenType::STAR);
                break;
            case '%':
                add_token(match('=') ? TokenType::PERCENT_EQUAL : TokenType::PERCENT);
                break;
            case '!':
                add_token(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
                break;
            case '=':
                if (match('=')) {
                    add_token(TokenType::EQUAL_EQUAL);
                } else if (match('>')) {
                    add_token(TokenType::ARROW);
                } else {
                    add_token(TokenType::EQUAL);
                }
                break;
            case '<':
                if (match('<')) {
                    add_token(TokenType::LEFT_SHIFT);
                } else {
                    add_token(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
                }
                break;
            case '>':
                if (match('>')) {
                    add_token(TokenType::RIGHT_SHIFT);
                } else {
                    add_token(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
                }
                break;
            case '/':
                if (match('/')) {
                    while (peek() != '\n' && current < source.length()) advance();
                } else if (match('=')) {
                    add_token(TokenType::SLASH_EQUAL);
                }
                else {
                    add_token(TokenType::SLASH);
                }
                break;
            case '&':
                add_token(match('&') ? TokenType::AND : TokenType::BITWISE_AND);
                break;
            case '|':
                add_token(match('|') ? TokenType::OR : TokenType::BITWISE_OR);
                break;
            case '^': add_token(TokenType::BITWISE_XOR); break;
            case '~': add_token(TokenType::BITWISE_NOT); break;
            case ':': add_token(TokenType::COLON); break;


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
                }
                else {
                    // handle error
                }
                break;
        }
    }

    char Lexer::advance() {
        return source[current++];
    }

    void Lexer::add_token(TokenType type) {
        add_token(type, "");
    }

    void Lexer::add_token(TokenType type, const std::string& literal) {
        std::string text = source.substr(start, current - start);
        if (!literal.empty()) {
            text = literal;
        }
        tokens.push_back({type, text, line});
    }

    bool Lexer::match(char expected) {
        if (current >= source.length()) return false;
        if (source[current] != expected) return false;

        current++;
        return true;
    }

    char Lexer::peek() {
        if (current >= source.length()) return '\0';
        return source[current];
    }

    char Lexer::peek_next() {
        if (current + 1 >= source.length()) return '\0';
        return source[current + 1];
    }

    void Lexer::string() {
        while (peek() != '"' && current < source.length()) {
            if (peek() == '\n') line++;
            advance();
        }

        if (current >= source.length()) {
            // handle unterminated string error
            return;
        }

        advance(); // The closing ".

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

    void Lexer::identifier() {
        while (isalnum(peek()) || peek() == '_') advance();

        std::string text = source.substr(start, current - start);
        TokenType type = keywords.count(text) ? keywords[text] : TokenType::IDENTIFIER;
        add_token(type);
    }

} // namespace nota
