#include "Lexer.h"

namespace nota {

    Lexer::Lexer(const std::string& source) : source_(source), position_(0) {}

    Token Lexer::getNextToken() {
        return {TokenType::EndOfFile, ""};
    }

} // namespace nota
