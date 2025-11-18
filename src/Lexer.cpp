#include "Lexer.h"
#include <cctype>
#include <map>

namespace nota {

    static std::map<std::string, TokenType> keywords = {
        {"let", TokenType::Let},
        {"mut", TokenType::Mut},
        {"if", TokenType::If},
        {"else", TokenType::Else},
        {"match", TokenType::Match},
        {"while", TokenType::While},
        {"for", TokenType::For},
        {"do", TokenType::Do},
        {"func", TokenType::Func},
        {"class", TokenType::Class},
        {"import", TokenType::Import},
        {"as", TokenType::As},
        {"package", TokenType::Package},
        {"return", TokenType::Return},
        {"true", TokenType::True},
        {"false", TokenType::False},
        {"end", TokenType::End},
        {"this", TokenType::This},
        {"none", TokenType::None},
    };

    Lexer::Lexer(const std::string& source)
        : source(source), current(0), line(1), column(1) {}

    char Lexer::peek() {
        if (current >= source.length()) {
            return '\0';
        }
        return source[current];
    }

    char Lexer::peek_next() {
        if (current + 1 >= source.length()) {
            return '\0';
        }
        return source[current + 1];
    }

    char Lexer::advance() {
        char c = peek();
        if (c != '\0') {
            current++;
            if (c == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
        }
        return c;
    }

    bool Lexer::match(char expected) {
        if (peek() == expected) {
            advance();
            return true;
        }
        return false;
    }

    Token Lexer::make_token(TokenType type, const std::string& lexeme) {
         return {type, lexeme, line, column - (int)lexeme.length()};
    }

    Token Lexer::next_token() {
        while (peek() == ' ' || peek() == '\r' || peek() == '\t') {
            advance();
        }

        int start = current;
        char c = advance();

        if (c == '\0') return make_token(TokenType::Eof, "");
        if (c == '\n') return make_token(TokenType::Newline, "\\n");

        switch (c) {
            // Single-character tokens
            case '(': return make_token(TokenType::LeftParen, "(");
            case ')': return make_token(TokenType::RightParen, ")");
            case '{': return make_token(TokenType::LeftBrace, "{");
            case '}': return make_token(TokenType::RightBrace, "}");
            case '[': return make_token(TokenType::LeftBracket, "[");
            case ']': return make_token(TokenType::RightBracket, "]");
            case ',': return make_token(TokenType::Comma, ",");
            case '.':
                if (isdigit(peek())) {
                    while (isdigit(peek())) advance();
                    return make_token(TokenType::Float, source.substr(start, current - start));
                }
                return make_token(TokenType::Dot, ".");
            case ';': return make_token(TokenType::Semicolon, ";");
            case '-': return make_token(match('=') ? TokenType::MinusEqual : (match('-') ? TokenType::MinusMinus : TokenType::Minus), source.substr(start, current - start));
            case '+': {
                if (match('=')) return make_token(TokenType::PlusEqual, "+=");
                if (match('+')) return make_token(TokenType::PlusPlus, "++");
                return make_token(TokenType::Plus, "+");
            }
            case '*': return make_token(match('=') ? TokenType::StarEqual : TokenType::Star, source.substr(start, current - start));
            case '%': return make_token(match('=') ? TokenType::PercentEqual : TokenType::Percent, source.substr(start, current - start));
            case '&': return make_token(match('&') ? TokenType::AmpersandAmpersand : TokenType::Ampersand, source.substr(start, current - start));
            case '|': return make_token(match('|') ? TokenType::PipePipe : TokenType::Pipe, source.substr(start, current - start));
            case '^': return make_token(TokenType::Caret, "^");
            case '~': return make_token(TokenType::Tilde, "~");
            case ':':
                if (match(':')) return make_token(TokenType::DoubleColon, "::");
                return make_token(TokenType::Colon, ":");
            case '_': return make_token(TokenType::Underscore, "_");

            // One or two character tokens
            case '!': return make_token(match('=') ? TokenType::BangEqual : TokenType::Bang, source.substr(start, current - start));
            case '=': return make_token(match('>') ? TokenType::FatArrow : (match('=') ? TokenType::EqualEqual : TokenType::Equal), source.substr(start, current - start));
            case '<': return make_token(match('<') ? TokenType::LessLess : (match('=') ? TokenType::LessEqual : TokenType::Less), source.substr(start, current - start));
            case '>': return make_token(match('>') ? TokenType::GreaterGreater : (match('=') ? TokenType::GreaterEqual : TokenType::Greater), source.substr(start, current - start));

            // Comments and slash
            case '/':
                if (match('/')) {
                    while (peek() != '\n' && peek() != '\0') advance();
                    return make_token(TokenType::Comment, source.substr(start, current - start));
                } else if (match('*')) {
                    while (peek() != '*' && peek() != '\0') {
                        if (peek() == '\n') advance();
                        else advance();
                    }
                    if(peek() != '\0') advance(); // Consume '*'
                    if(peek() != '\0') advance(); // Consume '/'
                    return make_token(TokenType::Comment, source.substr(start, current - start));
                } else {
                    return make_token(match('=') ? TokenType::SlashEqual : TokenType::Slash, source.substr(start, current - start));
                }

            // String literals
            case '"': {
                while (peek() != '"' && peek() != '\0') {
                    advance();
                }
                advance(); // Closing quote
                return make_token(TokenType::String, source.substr(start, current - start));
            }

            default:
                if (isdigit(c)) {
                    while (isdigit(peek())) advance();
                    if (peek() == '.' && isdigit(peek_next())) {
                        advance(); // consume '.'
                        while (isdigit(peek())) advance();
                        return make_token(TokenType::Float, source.substr(start, current - start));
                    }
                    return make_token(TokenType::Integer, source.substr(start, current - start));
                }
                if (isalpha(c) || c == '_') {
                    while (isalnum(peek()) || peek() == '_') advance();
                    std::string lexeme = source.substr(start, current - start);
                    auto it = keywords.find(lexeme);
                    if (it != keywords.end()) {
                        return make_token(it->second, lexeme);
                    }
                    return make_token(TokenType::Identifier, lexeme);
                }
        }

        return make_token(TokenType::Eof, "");
    }

} // namespace nota
