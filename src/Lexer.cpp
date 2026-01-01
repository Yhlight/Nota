#include "Lexer.hpp"

Lexer::Lexer(std::string source) : m_source(std::move(source)) {}

std::vector<Token> Lexer::scanTokens() {
    // For now, just return an empty list of tokens.
    // The actual scanning logic will be implemented in the next steps.
    return m_tokens;
}
