#include <gtest/gtest.h>
#include "Lexer.hpp"

TEST(LexerTest, EmptySource) {
    Lexer lexer("");
    auto tokens = lexer.scanTokens();
    ASSERT_EQ(tokens.size(), 1);
    ASSERT_EQ(tokens.back().type, TokenType::END_OF_FILE);
}

TEST(LexerTest, SingleCharacterTokens) {
    std::string source = R"({ } ( ) : ; . ,
.
)";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();

    // Expect 9 single-character tokens + 1 EOF token
    ASSERT_EQ(tokens.size(), 10);

    EXPECT_EQ(tokens[0].type, TokenType::LEFT_BRACE);
    EXPECT_EQ(tokens[0].lexeme, "{");
    EXPECT_EQ(tokens[0].line, 1);

    EXPECT_EQ(tokens[1].type, TokenType::RIGHT_BRACE);
    EXPECT_EQ(tokens[1].lexeme, "}");
    EXPECT_EQ(tokens[1].line, 1);

    EXPECT_EQ(tokens[2].type, TokenType::LEFT_PAREN);
    EXPECT_EQ(tokens[2].lexeme, "(");
    EXPECT_EQ(tokens[2].line, 1);

    EXPECT_EQ(tokens[3].type, TokenType::RIGHT_PAREN);
    EXPECT_EQ(tokens[3].lexeme, ")");
    EXPECT_EQ(tokens[3].line, 1);

    EXPECT_EQ(tokens[4].type, TokenType::COLON);
    EXPECT_EQ(tokens[4].lexeme, ":");
    EXPECT_EQ(tokens[4].line, 1);

    EXPECT_EQ(tokens[5].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[5].lexeme, ";");
    EXPECT_EQ(tokens[5].line, 1);

    EXPECT_EQ(tokens[6].type, TokenType::DOT);
    EXPECT_EQ(tokens[6].lexeme, ".");
    EXPECT_EQ(tokens[6].line, 1);

    EXPECT_EQ(tokens[7].type, TokenType::COMMA);
    EXPECT_EQ(tokens[7].lexeme, ",");
    EXPECT_EQ(tokens[7].line, 1);

    EXPECT_EQ(tokens[8].type, TokenType::DOT);
    EXPECT_EQ(tokens[8].lexeme, ".");
    EXPECT_EQ(tokens[8].line, 2);

    EXPECT_EQ(tokens[9].type, TokenType::END_OF_FILE);
}
