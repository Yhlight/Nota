#include "Lexer.hpp"

#include <cctype>

namespace nota {

    Lexer::Lexer(const std::string& source) : m_source(source) {}

    std::vector<Token> Lexer::scanTokens() {
        while (!isAtEnd()) {
            m_start = m_current;
            scanToken();
        }

        m_tokens.push_back({TokenType::END_OF_FILE, "", m_line, m_column});
        return m_tokens;
    }

    void Lexer::scanToken() {
        char c = advance();
        switch (c) {
            case '+': m_tokens.push_back({TokenType::PLUS, "+", m_line, m_column}); break;
            case '-': m_tokens.push_back({TokenType::MINUS, "-", m_line, m_column}); break;
            case '*': m_tokens.push_back({TokenType::STAR, "*", m_line, m_column}); break;
            case '/': m_tokens.push_back({TokenType::SLASH, "/", m_line, m_column}); break;
            case '(': m_tokens.push_back({TokenType::LEFT_PAREN, "(", m_line, m_column}); break;
            case ')': m_tokens.push_back({TokenType::RIGHT_PAREN, ")", m_line, m_column}); break;
            case ' ':
            case '\r':
            case '\t':
                // Ignore whitespace
                break;
            case '\n':
                m_line++;
                m_column = 1;
                break;
            default:
                if (std::isdigit(c)) {
                    number();
                } else if (std::isalpha(c) || c == '_') {
                    identifier();
                } else {
                    m_tokens.push_back({TokenType::UNEXPECTED, std::string(1, c), m_line, m_column});
                }
                break;
        }
    }

    char Lexer::advance() {
        m_column++;
        return m_source[m_current++];
    }

    bool Lexer::isAtEnd() {
        return m_current >= m_source.length();
    }

    char Lexer::peek() {
        if (isAtEnd()) return '\0';
        return m_source[m_current];
    }

    char Lexer::peekNext() {
        if (m_current + 1 >= m_source.length()) return '\0';
        return m_source[m_current + 1];
    }

    void Lexer::number() {
        while (std::isdigit(peek())) {
            advance();
        }

        if (peek() == '.' && std::isdigit(peekNext())) {
            advance();
            while (std::isdigit(peek())) {
                advance();
            }
        }

        m_tokens.push_back({TokenType::NUMBER, m_source.substr(m_start, m_current - m_start), m_line, m_column});
    }

    void Lexer::identifier() {
        while (std::isalnum(peek()) || peek() == '_') {
            advance();
        }

        m_tokens.push_back({TokenType::IDENTIFIER, m_source.substr(m_start, m_current - m_start), m_line, m_column});
    }

} // namespace nota
