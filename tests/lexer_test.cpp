#include <gtest/gtest.h>
#include "Lexer.hpp"

TEST(LexerTest, EmptySource) {
    Lexer lexer("");
    auto tokens = lexer.scanTokens();
    ASSERT_TRUE(tokens.empty());
}
