#include "doctest.h"
#include "Token.h"

TEST_CASE("Token can be created and its members can be accessed") {
    nota::Token token{nota::TokenType::INTEGER, "123", 1, 1};

    CHECK(token.type == nota::TokenType::INTEGER);
    CHECK(token.lexeme == "123");
    CHECK(token.line == 1);
    CHECK(token.column == 1);
}
