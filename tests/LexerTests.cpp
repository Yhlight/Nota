#include <gtest/gtest.h>
#include "Lexer.h"

TEST(LexerTest, RecognizesLeftBrace) {
    std::string source = "{";
    Lexer lexer(source);
    Token token = lexer.nextToken();
    EXPECT_EQ(token.type, TokenType::LeftBrace);
    EXPECT_EQ(token.lexeme, "{");
    EXPECT_EQ(token.line, 1);
}
