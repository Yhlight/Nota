#include <gtest/gtest.h>
#include "../src/lexer.hpp"

TEST(LexerTest, TokenizesKeywords) {
    std::string source = "let mut if";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[0].type, TokenType::LET);
    EXPECT_EQ(tokens[1].type, TokenType::MUT);
    EXPECT_EQ(tokens[2].type, TokenType::IF);
    EXPECT_EQ(tokens[3].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, TokenizesIdentifiers) {
    std::string source = "x y my_var";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[0].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[0].lexeme, "x");
    EXPECT_EQ(tokens[1].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[1].lexeme, "y");
    EXPECT_EQ(tokens[2].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[2].lexeme, "my_var");
    EXPECT_EQ(tokens[3].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, TokenizesNumbers) {
    std::string source = "123 45.67";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].type, TokenType::INTEGER);
    EXPECT_EQ(tokens[0].lexeme, "123");
    EXPECT_EQ(tokens[1].type, TokenType::FLOAT);
    EXPECT_EQ(tokens[1].lexeme, "45.67");
    EXPECT_EQ(tokens[2].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, TokenizesStrings) {
    std::string source = "\"hello world\"";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::STRING);
    EXPECT_EQ(tokens[0].lexeme, "\"hello world\"");
    EXPECT_EQ(tokens[1].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, TokenizesOperators) {
    std::string source = "+ - * / = == != < > <= >= : . -> ::";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 16);
    EXPECT_EQ(tokens[0].type, TokenType::PLUS);
    EXPECT_EQ(tokens[1].type, TokenType::MINUS);
    EXPECT_EQ(tokens[2].type, TokenType::STAR);
    EXPECT_EQ(tokens[3].type, TokenType::SLASH);
    EXPECT_EQ(tokens[4].type, TokenType::ASSIGN);
    EXPECT_EQ(tokens[5].type, TokenType::EQUAL);
    EXPECT_EQ(tokens[6].type, TokenType::NOT_EQUAL);
    EXPECT_EQ(tokens[7].type, TokenType::LESS);
    EXPECT_EQ(tokens[8].type, TokenType::GREATER);
    EXPECT_EQ(tokens[9].type, TokenType::LESS_EQUAL);
    EXPECT_EQ(tokens[10].type, TokenType::GREATER_EQUAL);
    EXPECT_EQ(tokens[11].type, TokenType::COLON);
    EXPECT_EQ(tokens[12].type, TokenType::DOT);
    EXPECT_EQ(tokens[13].type, TokenType::ARROW);
    EXPECT_EQ(tokens[14].type, TokenType::COLON_COLON);
    EXPECT_EQ(tokens[15].type, TokenType::END_OF_FILE);
}
