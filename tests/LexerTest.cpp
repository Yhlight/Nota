#include <gtest/gtest.h>
#include "Lexer.h"
#include "Token.h"
#include "TokenType.h"
#include <vector>

TEST(LexerTest, SimpleComponent) {
    std::string source = "App { width: 100 }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();

    ASSERT_EQ(tokens.size(), 7);

    EXPECT_EQ(tokens[0].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[0].lexeme, "App");

    EXPECT_EQ(tokens[1].type, TokenType::LEFT_BRACE);
    EXPECT_EQ(tokens[1].lexeme, "{");

    EXPECT_EQ(tokens[2].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[2].lexeme, "width");

    EXPECT_EQ(tokens[3].type, TokenType::COLON);
    EXPECT_EQ(tokens[3].lexeme, ":");

    EXPECT_EQ(tokens[4].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[4].lexeme, "100");
    EXPECT_EQ(std::any_cast<double>(tokens[4].literal), 100.0);

    EXPECT_EQ(tokens[5].type, TokenType::RIGHT_BRACE);
    EXPECT_EQ(tokens[5].lexeme, "}");

    EXPECT_EQ(tokens[6].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, Comments) {
    std::string source = R"(
        // This is a comment.
        App {
            /* This is a
             * block comment.
             */
            color: "#fff"
        }
    )";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();

    ASSERT_EQ(tokens.size(), 7);
    EXPECT_EQ(tokens[0].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[1].type, TokenType::LEFT_BRACE);
    EXPECT_EQ(tokens[2].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[3].type, TokenType::COLON);
    EXPECT_EQ(tokens[4].type, TokenType::STRING);
    EXPECT_EQ(tokens[5].type, TokenType::RIGHT_BRACE);
    EXPECT_EQ(tokens[6].type, TokenType::END_OF_FILE);
}
