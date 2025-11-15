#include "gtest/gtest.h"
#include "lexer.hpp"

TEST(LexerTest, TestSingleCharacterTokens) {
    std::string source = "(){},.+-*/";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    std::vector<TokenType> expected_tokens = {
        TokenType::LEFT_PAREN,
        TokenType::RIGHT_PAREN,
        TokenType::LEFT_BRACE,
        TokenType::RIGHT_BRACE,
        TokenType::COMMA,
        TokenType::DOT,
        TokenType::PLUS,
        TokenType::MINUS,
        TokenType::STAR,
        TokenType::SLASH,
        TokenType::END_OF_FILE
    };

    ASSERT_EQ(tokens.size(), expected_tokens.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected_tokens[i]);
    }
}

TEST(LexerTest, TestStringLiterals) {
    std::string source = "\"hello world\"";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::STRING);
    EXPECT_EQ(tokens[0].lexeme, "\"hello world\"");
}

TEST(LexerTest, TestNumberLiterals) {
    std::string source = "123 45.67";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[0].lexeme, "123");
    EXPECT_EQ(tokens[1].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[1].lexeme, "45.67");
}

TEST(LexerTest, TestIdentifiersAndKeywords) {
    std::string source = "let var foo = bar";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    std::vector<TokenType> expected_tokens = {
        TokenType::LET,
        TokenType::VAR,
        TokenType::IDENTIFIER,
        TokenType::EQUAL,
        TokenType::IDENTIFIER,
        TokenType::END_OF_FILE
    };

    ASSERT_EQ(tokens.size(), expected_tokens.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected_tokens[i]);
    }

    EXPECT_EQ(tokens[0].lexeme, "let");
    EXPECT_EQ(tokens[1].lexeme, "var");
    EXPECT_EQ(tokens[2].lexeme, "foo");
    EXPECT_EQ(tokens[4].lexeme, "bar");
}

TEST(LexerTest, TestComments) {
    std::string source = "// this is a comment\nlet foo = bar // another comment";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    std::vector<TokenType> expected_tokens = {
        TokenType::LET,
        TokenType::IDENTIFIER,
        TokenType::EQUAL,
        TokenType::IDENTIFIER,
        TokenType::END_OF_FILE
    };

    ASSERT_EQ(tokens.size(), expected_tokens.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected_tokens[i]);
    }
}

TEST(LexerTest, TestOneOrTwoCharacterTokens) {
    std::string source = "! != = == > >= < <=";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    std::vector<TokenType> expected_tokens = {
        TokenType::BANG,
        TokenType::BANG_EQUAL,
        TokenType::EQUAL,
        TokenType::EQUAL_EQUAL,
        TokenType::GREATER,
        TokenType::GREATER_EQUAL,
        TokenType::LESS,
        TokenType::LESS_EQUAL,
        TokenType::END_OF_FILE
    };

    ASSERT_EQ(tokens.size(), expected_tokens.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected_tokens[i]);
    }
}
