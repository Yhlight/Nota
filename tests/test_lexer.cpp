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

TEST(LexerTest, MultilineCommentLineCounting) {
    std::string input = "App /* line 1 \n line 2 \n line 3 */ {";
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    // App (Line 1)
    // \n 1 (line 2)
    // \n 2 (line 3)
    // line 3... */ { (Line 3)

    // Wait, if input is "App /* line 1 \n line 2 \n line 3 */ {"
    // Line 1: App /* line 1
    // Line 2:  line 2
    // Line 3:  line 3 */ {

    EXPECT_EQ(tokens[0].value, "App");
    EXPECT_EQ(tokens[0].line, 1);

    EXPECT_EQ(tokens[1].type, TokenType::LBrace);
    EXPECT_EQ(tokens[1].line, 3);
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

TEST(LexerTest, Operators) {
    std::string input = "+ - * / == != < > <= >= && || !";
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    std::vector<TokenType> expected = {
        TokenType::Plus, TokenType::Minus, TokenType::Star, TokenType::Slash,
        TokenType::EqualEqual, TokenType::BangEqual,
        TokenType::Less, TokenType::Greater, TokenType::LessEqual, TokenType::GreaterEqual,
        TokenType::AmpAmp, TokenType::PipePipe, TokenType::Bang,
        TokenType::EndOfFile
    };

    ASSERT_EQ(tokens.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected[i]) << "Mismatch at index " << i;
    }
}

TEST(LexerTest, Keywords) {
    std::string input = "if else true false import package";
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    EXPECT_EQ(tokens[0].type, TokenType::If);
    EXPECT_EQ(tokens[1].type, TokenType::Else);
    EXPECT_EQ(tokens[2].type, TokenType::True);
    EXPECT_EQ(tokens[3].type, TokenType::False);
    EXPECT_EQ(tokens[4].type, TokenType::Import);
    EXPECT_EQ(tokens[5].type, TokenType::Package);
}
