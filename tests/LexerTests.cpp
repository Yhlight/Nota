#include <gtest/gtest.h>
#include "../src/Lexer.hpp"
#include "../src/Token.hpp"
#include <vector>

TEST(LexerTest, TokenizesBasicComponent) {
    std::string source = "App {\n  width: 100%\n}";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 7);
    EXPECT_EQ(tokens[0].type, TokenType::APP);
    EXPECT_EQ(tokens[1].type, TokenType::LEFT_BRACE);
    EXPECT_EQ(tokens[2].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[2].lexeme, "width");
    EXPECT_EQ(tokens[3].type, TokenType::COLON);
    EXPECT_EQ(tokens[4].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[4].lexeme, "100%");
    EXPECT_EQ(tokens[5].type, TokenType::RIGHT_BRACE);
    EXPECT_EQ(tokens[6].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, HandlesAllSingleCharTokens) {
    std::string source = "{}[]:.*=#";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 10);
    EXPECT_EQ(tokens[0].type, TokenType::LEFT_BRACE);
    EXPECT_EQ(tokens[1].type, TokenType::RIGHT_BRACE);
    EXPECT_EQ(tokens[2].type, TokenType::LEFT_BRACKET);
    EXPECT_EQ(tokens[3].type, TokenType::RIGHT_BRACKET);
    EXPECT_EQ(tokens[4].type, TokenType::COLON);
    EXPECT_EQ(tokens[5].type, TokenType::DOT);
    EXPECT_EQ(tokens[6].type, TokenType::STAR);
    EXPECT_EQ(tokens[7].type, TokenType::EQUAL);
    EXPECT_EQ(tokens[8].type, TokenType::HASH);
    EXPECT_EQ(tokens[9].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, TokenizesLiterals) {
    std::string source = R"( "hello" 123 45.67 )";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[0].type, TokenType::STRING);
    EXPECT_EQ(tokens[0].lexeme, "\"hello\"");
    EXPECT_EQ(tokens[1].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[1].lexeme, "123");
    EXPECT_EQ(tokens[2].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[2].lexeme, "45.67");
    EXPECT_EQ(tokens[3].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, IgnoresComments) {
    std::string source = R"(
        // This is a single line comment
        Rect { /* this is a
                  block comment */ }
    )";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[0].type, TokenType::RECT);
    EXPECT_EQ(tokens[1].type, TokenType::LEFT_BRACE);
    EXPECT_EQ(tokens[2].type, TokenType::RIGHT_BRACE);
    EXPECT_EQ(tokens[3].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, HandlesBlockCommentWithAsterisks) {
    std::string source = "/* this is a * test */";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, HandlesComplexNestedStructure) {
    std::string source = R"(
        App {
            Row {
                id: header;
                Text { text: "Hello Nota!" }
            }
        }
    )";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    std::vector<TokenType> expected_types = {
        TokenType::APP, TokenType::LEFT_BRACE,
        TokenType::ROW, TokenType::LEFT_BRACE,
        TokenType::IDENTIFIER, TokenType::COLON, TokenType::IDENTIFIER, TokenType::SEMICOLON,
        TokenType::TEXT, TokenType::LEFT_BRACE,
        TokenType::IDENTIFIER, TokenType::COLON, TokenType::STRING,
        TokenType::RIGHT_BRACE,
        TokenType::RIGHT_BRACE,
        TokenType::RIGHT_BRACE,
        TokenType::END_OF_FILE
    };

    ASSERT_EQ(tokens.size(), expected_types.size());
    for(size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected_types[i]);
    }
}
