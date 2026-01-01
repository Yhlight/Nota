#include "Lexer.hpp"

#include <utility>

Lexer::Lexer(std::string source) : m_source(std::move(source)) {}

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        // We are at the beginning of the next lexeme.
        m_start = m_current;
        scanToken();
    }

    m_tokens.push_back({TokenType::END_OF_FILE, "", m_line});
    return m_tokens;
}

void Lexer::scanToken() {
    char c = advance();
    switch (c) {
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;
        case '(': addToken(TokenType::LEFT_PAREN); break;
        case ')': addToken(TokenType::RIGHT_PAREN); break;
        case ':': addToken(TokenType::COLON); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case '.': addToken(TokenType::DOT); break;
        case ',': addToken(TokenType::COMMA); break;

        // Ignore whitespace.
        case ' ':
        case '\r':
        case '\t':
            break;

        case '\n':
            m_line++;
            break;

        default:
            // For now, we'll treat everything else as unexpected.
            addToken(TokenType::UNEXPECTED);
            break;
    }
}

bool Lexer::isAtEnd() {
    return m_current >= m_source.length();
}

char Lexer::advance() {
    return m_source[m_current++];
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return m_source[m_current];
}

void Lexer::addToken(TokenType type) {
    addToken(type, "");
}

void Lexer::addToken(TokenType type, const std::string& literal) {
    std::string text = m_source.substr(m_start, m_current - m_start);
    m_tokens.push_back({type, text, m_line});
}
