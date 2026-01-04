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
