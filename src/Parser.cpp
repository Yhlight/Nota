#include "Parser.h"

namespace nota {

    Parser::Parser(Lexer& lexer)
        : lexer(lexer), current_token(lexer.next_token()) {}

    void Parser::parse() {
        // Nothing to parse yet
    }

} // namespace nota
