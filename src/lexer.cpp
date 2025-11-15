#include "lexer.hpp"

namespace nota {

    Lexer::Lexer(const std::string& source) : source(source) {}

    std::vector<Token> Lexer::scan_tokens() {
        // ... to be implemented
        return tokens;
    }

} // namespace nota
