#include <gtest/gtest.h>
#include "Lexer.h"

TEST(LexerTest, IgnoresComments) {
    std::string source = R"(
        // This is a single-line comment.
        /* This is a
           multi-line comment. */
    )";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, ScansSingleCharacterTokens) {
    std::string source = "(){},.-+;*/";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    std::vector<TokenType> expected_types = {
        TokenType::LEFT_PAREN, TokenType::RIGHT_PAREN,
        TokenType::LEFT_BRACE, TokenType::RIGHT_BRACE,
        TokenType::COMMA, TokenType::DOT, TokenType::MINUS,
        TokenType::PLUS, TokenType::SEMICOLON, TokenType::STAR,
        TokenType::SLASH, TokenType::END_OF_FILE
    };

    ASSERT_EQ(tokens.size(), expected_types.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected_types[i]);
    }
}

TEST(LexerTest, ScansStringLiterals) {
    std::string source = "\"hello world\"";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::STRING);
    EXPECT_EQ(std::get<std::string>(tokens[0].literal), "hello world");
}

TEST(LexerTest, ScansNumberLiterals) {
    std::string source = "123 123.456";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].type, TokenType::NUMBER);
    EXPECT_EQ(std::get<double>(tokens[0].literal), 123);
    EXPECT_EQ(tokens[1].type, TokenType::NUMBER);
    EXPECT_EQ(std::get<double>(tokens[1].literal), 123.456);
}

TEST(LexerTest, ScansIdentifiersAndKeywords) {
    std::string source = "var language = \"Nota\";";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    std::vector<TokenType> expected_types = {
        TokenType::VAR,
        TokenType::IDENTIFIER,
        TokenType::EQUAL,
        TokenType::STRING,
        TokenType::SEMICOLON,
        TokenType::END_OF_FILE
    };

    ASSERT_EQ(tokens.size(), expected_types.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected_types[i]);
    }
    EXPECT_EQ(tokens[1].lexeme, "language");
    EXPECT_EQ(std::get<std::string>(tokens[3].literal), "Nota");
}

TEST(LexerTest, ScansTwoCharacterTokens) {
    std::string source = "! != = == > >= < <=";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    std::vector<TokenType> expected_types = {
        TokenType::BANG, TokenType::BANG_EQUAL,
        TokenType::EQUAL, TokenType::EQUAL_EQUAL,
        TokenType::GREATER, TokenType::GREATER_EQUAL,
        TokenType::LESS, TokenType::LESS_EQUAL,
        TokenType::END_OF_FILE
    };

    ASSERT_EQ(tokens.size(), expected_types.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected_types[i]);
    }
}
