#include <gtest/gtest.h>
#include "Lexer.hpp"

TEST(LexerTest, TestSingleCharTokens) {
    std::string source = "{}( ):;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 7);
    EXPECT_EQ(tokens[0].type, TokenType::LEFT_BRACE);
    EXPECT_EQ(tokens[1].type, TokenType::RIGHT_BRACE);
    EXPECT_EQ(tokens[2].type, TokenType::LEFT_PAREN);
    EXPECT_EQ(tokens[3].type, TokenType::RIGHT_PAREN);
    EXPECT_EQ(tokens[4].type, TokenType::COLON);
    EXPECT_EQ(tokens[5].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[6].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, TestKeywordsAndIdentifiers) {
    std::string source = "Item App myVar";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[0].type, TokenType::ITEM);
    EXPECT_EQ(tokens[0].lexeme, "Item");
    EXPECT_EQ(tokens[1].type, TokenType::APP);
    EXPECT_EQ(tokens[1].lexeme, "App");
    EXPECT_EQ(tokens[2].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[2].lexeme, "myVar");
    EXPECT_EQ(tokens[3].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, TestStringAndNumberLiterals) {
    std::string source = "\"hello\" 123.45";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].type, TokenType::STRING);
    EXPECT_EQ(tokens[0].lexeme, "\"hello\"");
    EXPECT_EQ(tokens[1].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[1].lexeme, "123.45");
    EXPECT_EQ(tokens[2].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, TestCommentsAndWhitespace) {
    std::string source = R"(
        // This is a single line comment
        {
            /* This is a
               multi-line comment */
        }
    )";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].type, TokenType::LEFT_BRACE);
    EXPECT_EQ(tokens[1].type, TokenType::RIGHT_BRACE);
    EXPECT_EQ(tokens[2].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, UnterminatedMultiLineCommentAtEOF) {
    std::string source = "/* unterminated";
    Lexer lexer(source);
    // This test's primary purpose is to ensure the lexer does not crash.
    // A secondary benefit is checking that it produces a clean token list.
    std::vector<Token> tokens = lexer.scanTokens();
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, TokenType::END_OF_FILE);
}
