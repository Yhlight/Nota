#include <gtest/gtest.h>
#include "lexer/Lexer.h"
#include "lexer/Token.h"

TEST(LexerTest, TokenizesBasicOperators) {
    Lexer lexer("+ - * /");
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 5);
    EXPECT_EQ(tokens[0].type, TokenType::PLUS);
    EXPECT_EQ(tokens[1].type, TokenType::MINUS);
    EXPECT_EQ(tokens[2].type, TokenType::ASTERISK);
    EXPECT_EQ(tokens[3].type, TokenType::SLASH);
    EXPECT_EQ(tokens[4].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, TokenizesIntegerLiterals) {
    Lexer lexer("123 456");
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].type, TokenType::INTEGER);
    EXPECT_EQ(std::get<int>(tokens[0].literal), 123);
    EXPECT_EQ(tokens[1].type, TokenType::INTEGER);
    EXPECT_EQ(std::get<int>(tokens[1].literal), 456);
    EXPECT_EQ(tokens[2].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, TokenizesKeywordsAndIdentifiers) {
    Lexer lexer("let x = 10");
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 5);
    EXPECT_EQ(tokens[0].type, TokenType::LET);
    EXPECT_EQ(tokens[1].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[2].type, TokenType::ASSIGN);
    EXPECT_EQ(tokens[3].type, TokenType::INTEGER);
    EXPECT_EQ(tokens[4].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, TokenizesComparisonAndLogicalOperators) {
    Lexer lexer("== != < <= > >= !");
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 8);
    EXPECT_EQ(tokens[0].type, TokenType::EQUALS);
    EXPECT_EQ(tokens[1].type, TokenType::NOT_EQUALS);
    EXPECT_EQ(tokens[2].type, TokenType::LESS);
    EXPECT_EQ(tokens[3].type, TokenType::LESS_EQUALS);
    EXPECT_EQ(tokens[4].type, TokenType::GREATER);
    EXPECT_EQ(tokens[5].type, TokenType::GREATER_EQUALS);
    EXPECT_EQ(tokens[6].type, TokenType::LOGICAL_NOT);
    EXPECT_EQ(tokens[7].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, TokenizesFloatLiterals) {
    Lexer lexer("123.456 0.5");
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].type, TokenType::FLOAT);
    EXPECT_EQ(std::get<double>(tokens[0].literal), 123.456);
    EXPECT_EQ(tokens[1].type, TokenType::FLOAT);
    EXPECT_EQ(std::get<double>(tokens[1].literal), 0.5);
    EXPECT_EQ(tokens[2].type, TokenType::END_OF_FILE);
}
