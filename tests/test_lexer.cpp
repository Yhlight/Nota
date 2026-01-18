#include <gtest/gtest.h>
#include "Lexer.hpp"
#include "Token.hpp"

TEST(LexerTest, TokenizeSimpleItem) {
    std::string source = "Item { }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 4);

    EXPECT_EQ(tokens[0].type, TokenType::ITEM);
    EXPECT_EQ(tokens[0].lexeme, "Item");
    EXPECT_EQ(tokens[0].line, 1);

    EXPECT_EQ(tokens[1].type, TokenType::LEFT_BRACE);
    EXPECT_EQ(tokens[1].lexeme, "{");
    EXPECT_EQ(tokens[1].line, 1);

    EXPECT_EQ(tokens[2].type, TokenType::RIGHT_BRACE);
    EXPECT_EQ(tokens[2].lexeme, "}");
    EXPECT_EQ(tokens[2].line, 1);

    EXPECT_EQ(tokens[3].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, TokenizeWithCommentsAndLiterals) {
    std::string source = R"(
// This is a comment
App {
    text: "Hello, Nota!" // Another comment
    width: 100%
    height: 25.5
    /*
        Multi-line comment
    */
}
)";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    // The final token list should be:
    // APP, LBRACE, IDENT, COLON, STRING, IDENT, COLON, NUMBER, UNKNOWN(%),
    // IDENT, COLON, NUMBER, RBRACE, EOF
    ASSERT_EQ(tokens.size(), 14);

    EXPECT_EQ(tokens[0].type, TokenType::APP);
    EXPECT_EQ(tokens[1].type, TokenType::LEFT_BRACE);
    EXPECT_EQ(tokens[2].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[2].lexeme, "text");
    EXPECT_EQ(tokens[3].type, TokenType::COLON);
    EXPECT_EQ(tokens[4].type, TokenType::STRING);
    EXPECT_EQ(tokens[4].lexeme, "\"Hello, Nota!\"");
    EXPECT_EQ(tokens[5].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[5].lexeme, "width");
    EXPECT_EQ(tokens[6].type, TokenType::COLON);
    EXPECT_EQ(tokens[7].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[7].lexeme, "100");
    // The '%' is now a first-class token.
    EXPECT_EQ(tokens[8].type, TokenType::PERCENT);
    EXPECT_EQ(tokens[9].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[9].lexeme, "height");
    EXPECT_EQ(tokens[10].type, TokenType::COLON);
    EXPECT_EQ(tokens[11].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[11].lexeme, "25.5");
    EXPECT_EQ(tokens[12].type, TokenType::RIGHT_BRACE);
    EXPECT_EQ(tokens[13].type, TokenType::END_OF_FILE);
}
