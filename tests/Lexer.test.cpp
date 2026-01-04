#include <gtest/gtest.h>
#include "Lexer.h"
#include "Token.h"
#include <vector>

TEST(LexerTest, TokenizesSimpleApp) {
    std::string source = "App {}";
    nota::Lexer lexer(source);
    std::vector<nota::Token> tokens = lexer.scan_tokens();

    ASSERT_EQ(tokens.size(), 4);

    EXPECT_EQ(tokens[0].type, nota::TokenType::APP);
    EXPECT_EQ(tokens[0].lexeme, "App");

    EXPECT_EQ(tokens[1].type, nota::TokenType::LEFT_BRACE);
    EXPECT_EQ(tokens[1].lexeme, "{");

    EXPECT_EQ(tokens[2].type, nota::TokenType::RIGHT_BRACE);
    EXPECT_EQ(tokens[2].lexeme, "}");

    EXPECT_EQ(tokens[3].type, nota::TokenType::END_OF_FILE);
}

TEST(LexerTest, TokenizesStringProperty) {
    std::string source = "text: \"Hello\"";
    nota::Lexer lexer(source);
    std::vector<nota::Token> tokens = lexer.scan_tokens();

    ASSERT_EQ(tokens.size(), 4);

    EXPECT_EQ(tokens[0].type, nota::TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[0].lexeme, "text");

    EXPECT_EQ(tokens[1].type, nota::TokenType::COLON);
    EXPECT_EQ(tokens[1].lexeme, ":");

    EXPECT_EQ(tokens[2].type, nota::TokenType::STRING);
    EXPECT_EQ(tokens[2].lexeme, "\"Hello\"");

    EXPECT_EQ(tokens[3].type, nota::TokenType::END_OF_FILE);
}

TEST(LexerTest, TokenizesNumericProperty) {
    std::string source = "width: 100";
    nota::Lexer lexer(source);
    std::vector<nota::Token> tokens = lexer.scan_tokens();

    ASSERT_EQ(tokens.size(), 4);

    EXPECT_EQ(tokens[0].type, nota::TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[0].lexeme, "width");

    EXPECT_EQ(tokens[1].type, nota::TokenType::COLON);
    EXPECT_EQ(tokens[1].lexeme, ":");

    EXPECT_EQ(tokens[2].type, nota::TokenType::NUMBER);
    EXPECT_EQ(tokens[2].lexeme, "100");

    EXPECT_EQ(tokens[3].type, nota::TokenType::END_OF_FILE);
}
