#include "lexer.hpp"
#include <unordered_map>
#include <cctype>

namespace nota {

    static std::unordered_map<std::string, TokenType> keywords;

    static void initializeKeywords() {
        if (keywords.empty()) {
            keywords["let"] = TokenType::LET;
            keywords["mut"] = TokenType::MUT;
            keywords["true"] = TokenType::TRUE;
            keywords["false"] = TokenType::FALSE;
            keywords["if"] = TokenType::IF;
            keywords["else"] = TokenType::ELSE;
            keywords["while"] = TokenType::WHILE;
            keywords["for"] = TokenType::FOR;
            keywords["do"] = TokenType::DO;
            keywords["func"] = TokenType::FUNC;
            keywords["class"] = TokenType::CLASS;
            keywords["return"] = TokenType::RETURN;
            keywords["super"] = TokenType::SUPER;
            keywords["this"] = TokenType::THIS;
            keywords["import"] = TokenType::IMPORT;
            keywords["as"] = TokenType::AS;
            keywords["package"] = TokenType::PACKAGE;
            keywords["match"] = TokenType::MATCH;
            keywords["end"] = TokenType::END;
        }
    }

    Lexer::Lexer(const std::string& source) : source(source) {
        initializeKeywords();
    }

    bool Lexer::isAtEnd() {
        return current >= source.length();
    }

    char Lexer::advance() {
        return source[current++];
    }

    char Lexer::peek() {
        if (isAtEnd()) return '\0';
        return source[current];
    }

    char Lexer::peekNext() {
        if (current + 1 >= source.length()) return '\0';
        return source[current + 1];
    }

    bool Lexer::match(char expected) {
        if (isAtEnd()) return false;
        if (source[current] != expected) return false;
        current++;
        return true;
    }

    Token Lexer::makeToken(TokenType type) const {
        return Token{type, source.substr(start, current - start), line};
    }

    Token Lexer::makeToken(TokenType type, const std::string& lexeme) const {
        return Token{type, lexeme, line};
    }

    Token Lexer::errorToken(const std::string& message) const {
        return Token{TokenType::ERROR, message, line};
    }

    Token Lexer::string() {
        while (peek() != '"' && !isAtEnd()) {
            if (peek() == '\n') line++;
            advance();
        }

        if (isAtEnd()) {
            return errorToken("Unterminated string.");
        }

        advance(); // The closing ".
        return makeToken(TokenType::STRING, source.substr(start + 1, current - start - 2));
    }

    Token Lexer::number() {
        while (std::isdigit(peek())) advance();

        if (peek() == '.' && std::isdigit(peekNext())) {
            advance();
            while (std::isdigit(peek())) advance();
        }

        return makeToken(TokenType::NUMBER);
    }

    TokenType Lexer::identifierType() {
        auto it = keywords.find(source.substr(start, current - start));
        if (it != keywords.end()) {
            return it->second;
        }
        return TokenType::IDENTIFIER;
    }

    Token Lexer::identifier() {
        while (std::isalnum(peek()) || peek() == '_') advance();
        return makeToken(identifierType());
    }

    void Lexer::skipWhitespace() {
        for (;;) {
            char c = peek();
            switch (c) {
                case ' ':
                case '\r':
                case '\t':
                    advance();
                    break;
                case '\n':
                    line++;
                    advance();
                    break;
                case '#': // Assuming '#' for comments
                    while (peek() != '\n' && !isAtEnd()) advance();
                    break;
                default:
                    return;
            }
        }
    }

    Token Lexer::scanToken() {
        skipWhitespace();
        start = current;

        if (isAtEnd()) return makeToken(TokenType::END_OF_FILE, "");

        char c = advance();

        if (std::isalpha(c) || c == '_') return identifier();
        if (std::isdigit(c)) return number();

        switch (c) {
            case '(': return makeToken(TokenType::LEFT_PAREN);
            case ')': return makeToken(TokenType::RIGHT_PAREN);
            case '{': return makeToken(TokenType::LEFT_BRACE);
            case '}': return makeToken(TokenType::RIGHT_BRACE);
            case '[': return makeToken(TokenType::LEFT_BRACKET);
            case ']': return makeToken(TokenType::RIGHT_BRACKET);
            case ',': return makeToken(TokenType::COMMA);
            case '.': return makeToken(TokenType::DOT);
            case '-': return makeToken(match('=') ? TokenType::MINUS_EQUAL : TokenType::MINUS);
            case '+': return makeToken(match('=') ? TokenType::PLUS_EQUAL : TokenType::PLUS);
            case ';': return makeToken(TokenType::SEMICOLON);
            case '/': return makeToken(match('=') ? TokenType::SLASH_EQUAL : TokenType::SLASH);
            case '*': return makeToken(match('=') ? TokenType::STAR_EQUAL : TokenType::STAR);
            case '%': return makeToken(match('=') ? TokenType::PERCENT_EQUAL : TokenType::PERCENT);
            case ':': return makeToken(TokenType::COLON);
            case '!': return makeToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
            case '=': return makeToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
            case '<':
                if (match('=')) return makeToken(TokenType::LESS_EQUAL);
                if (match('<')) return makeToken(TokenType::LEFT_SHIFT);
                return makeToken(TokenType::LESS);
            case '>':
                if (match('=')) return makeToken(TokenType::GREATER_EQUAL);
                if (match('>')) return makeToken(TokenType::RIGHT_SHIFT);
                return makeToken(TokenType::GREATER);
            case '&': return makeToken(match('&') ? TokenType::AMPERSAND_AMPERSAND : TokenType::AMPERSAND);
            case '|': return makeToken(match('|') ? TokenType::PIPE_PIPE : TokenType::PIPE);
            case '^': return makeToken(TokenType::CARET);
            case '~': return makeToken(TokenType::TILDE);
            case '"': return string();
        }

        return errorToken("Unexpected character.");
    }

} // namespace nota
