#include <gtest/gtest.h>
#include "lexer/Lexer.h"
#include "lexer/Token.h"

#include <vector>

TEST(LexerTest, EmptySource) {
    nota::Lexer lexer("");
    std::vector<nota::Token> tokens = lexer.tokenize();
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, nota::TokenType::EndOfFile);
}

TEST(LexerTest, SimpleComponent) {
    nota::Lexer lexer("App {}");
    std::vector<nota::Token> tokens = lexer.tokenize();
    ASSERT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[0].type, nota::TokenType::Identifier);
    EXPECT_EQ(tokens[0].text, "App");
    EXPECT_EQ(tokens[1].type, nota::TokenType::LBrace);
    EXPECT_EQ(tokens[2].type, nota::TokenType::RBrace);
    EXPECT_EQ(tokens[3].type, nota::TokenType::EndOfFile);
}

TEST(LexerTest, ComponentWithProperty) {
    nota::Lexer lexer("Rect { width: 100 }");
    std::vector<nota::Token> tokens = lexer.tokenize();
    ASSERT_EQ(tokens.size(), 7);
    EXPECT_EQ(tokens[0].type, nota::TokenType::Identifier);
    EXPECT_EQ(tokens[0].text, "Rect");
    EXPECT_EQ(tokens[1].type, nota::TokenType::LBrace);
    EXPECT_EQ(tokens[2].type, nota::TokenType::Identifier);
    EXPECT_EQ(tokens[2].text, "width");
    EXPECT_EQ(tokens[3].type, nota::TokenType::Colon);
    EXPECT_EQ(tokens[4].type, nota::TokenType::Integer);
    EXPECT_EQ(tokens[4].text, "100");
    EXPECT_EQ(tokens[5].type, nota::TokenType::RBrace);
    EXPECT_EQ(tokens[6].type, nota::TokenType::EndOfFile);
}
