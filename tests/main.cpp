#include <gtest/gtest.h>
#include "../src/Lexer.h"

TEST(LexerTest, SingleLineComment) {
    std::string source = "// this is a comment\n"
                         "// this is another comment";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    ASSERT_TRUE(tokens.empty());
}

TEST(LexerTest, MultiLineComment) {
    std::string source = "/* this is a multi-line comment */";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    ASSERT_TRUE(tokens.empty());
}

TEST(LexerTest, KeywordsAndIdentifiers) {
    std::string source = "let mut variable";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    std::vector<Token> expected = {
        {TokenType::Let, "let"},
        {TokenType::Mut, "mut"},
        {TokenType::Identifier, "variable"},
    };
    ASSERT_EQ(tokens, expected);
}

TEST(LexerTest, InvalidNumber) {
    std::string source = "1.2.3";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    std::vector<Token> expected = {
        {TokenType::Number, "1.2"},
        {TokenType::Unknown, "."},
        {TokenType::Number, "3"},
    };
    ASSERT_EQ(tokens, expected);
}

TEST(LexerTest, Numbers) {
    std::string source = "123 45.67";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    std::vector<Token> expected = {
        {TokenType::Number, "123"},
        {TokenType::Number, "45.67"},
    };
    ASSERT_EQ(tokens, expected);
}

TEST(LexerTest, Operators) {
    std::string source = "+ - * / %";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    std::vector<Token> expected = {
        {TokenType::Plus, "+"},
        {TokenType::Minus, "-"},
        {TokenType::Asterisk, "*"},
        {TokenType::Slash, "/"},
        {TokenType::Percent, "%"},
    };
    ASSERT_EQ(tokens, expected);
}
