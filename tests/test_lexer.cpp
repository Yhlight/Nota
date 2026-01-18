#include <gtest/gtest.h>
#include "lexer.h"

TEST(LexerTest, BasicTokens) {
    nota::Lexer lexer("App { width: 100 }");
    auto tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 7);
    EXPECT_EQ(tokens[0].type, nota::TokenType::Identifier);
    EXPECT_EQ(tokens[0].text, "App");
    EXPECT_EQ(tokens[1].type, nota::TokenType::LBrace);
    EXPECT_EQ(tokens[2].type, nota::TokenType::Identifier);
    EXPECT_EQ(tokens[2].text, "width");
    EXPECT_EQ(tokens[3].type, nota::TokenType::Colon);
    EXPECT_EQ(tokens[4].type, nota::TokenType::Number);
    EXPECT_EQ(tokens[4].text, "100");
    EXPECT_EQ(tokens[5].type, nota::TokenType::RBrace);
    EXPECT_EQ(tokens[6].type, nota::TokenType::EndOfFile);
}

TEST(LexerTest, StringAndColor) {
    nota::Lexer lexer("color: \"red\" #ff0000");
    auto tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 5);
    EXPECT_EQ(tokens[2].type, nota::TokenType::String);
    EXPECT_EQ(tokens[2].text, "red");
    EXPECT_EQ(tokens[3].type, nota::TokenType::String);
    EXPECT_EQ(tokens[3].text, "#ff0000");
}
