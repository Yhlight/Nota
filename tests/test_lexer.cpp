#include <gtest/gtest.h>
#include "lexer/Lexer.h"

using namespace nota;

TEST(LexerTest, ReturnsUnknownTokenForUnrecognizedCharacter) {
    Lexer lexer("?");
    Token token = lexer.getNextToken();
    EXPECT_EQ(token.type, TokenType::Unknown);
    EXPECT_EQ(token.value, "?");
}
