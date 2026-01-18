#include <gtest/gtest.h>
#include "Lexer.h"

TEST(LexerTest, BasicTokens) {
    std::string input = "App { width: 100% }";
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    EXPECT_EQ(tokens.size(), 7); // App, {, width, :, 100%, }, EOF
    EXPECT_EQ(tokens[0].type, TokenType::Identifier);
    EXPECT_EQ(tokens[0].value, "App");
    EXPECT_EQ(tokens[1].type, TokenType::LBrace);
    EXPECT_EQ(tokens[2].type, TokenType::Identifier);
    EXPECT_EQ(tokens[2].value, "width");
    EXPECT_EQ(tokens[3].type, TokenType::Colon);
    EXPECT_EQ(tokens[4].type, TokenType::Number);
    EXPECT_EQ(tokens[4].value, "100%");
    EXPECT_EQ(tokens[5].type, TokenType::RBrace);
}

TEST(LexerTest, Comments) {
    std::string input = "App /* comment */ { // inline \n }";
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    EXPECT_EQ(tokens.size(), 4); // App, {, }, EOF
    EXPECT_EQ(tokens[0].value, "App");
}

TEST(LexerTest, Colors) {
    std::string input = "color: #ff0000";
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    EXPECT_EQ(tokens[2].type, TokenType::Identifier);
    EXPECT_EQ(tokens[2].value, "#ff0000");
}

TEST(LexerTest, Strings) {
    std::string input = "text: \"Hello World\"";
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    // Identifier(text), Colon, String(Hello World), EOF
    EXPECT_EQ(tokens[2].type, TokenType::String);
    EXPECT_EQ(tokens[2].value, "Hello World"); // Quotes stripped
}
