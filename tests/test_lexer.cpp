#include "vendor/doctest.h"
#include "../src/lib/Lexer.h"
#include <vector>

TEST_CASE("testing the lexer") {
    std::string source = "let x = 10;";
    nota::Lexer lexer(source);
    std::vector<nota::Token> tokens = lexer.scanTokens();

    CHECK(tokens.size() == 6);
    CHECK(tokens[0].type == nota::TokenType::LET);
    CHECK(tokens[1].type == nota::TokenType::IDENTIFIER);
    CHECK(tokens[2].type == nota::TokenType::EQUAL);
    CHECK(tokens[3].type == nota::TokenType::NUMBER);
    CHECK(tokens[4].type == nota::TokenType::SEMICOLON);
    CHECK(tokens[5].type == nota::TokenType::END_OF_FILE);
}
