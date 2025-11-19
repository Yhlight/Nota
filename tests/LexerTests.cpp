#include <gtest/gtest.h>
#include "Lexer.h"
#include <vector>

TEST(LexerTest, HandlesEmptySource) {
    Lexer lexer("");
    std::vector<Token> tokens = lexer.scanTokens();
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, HandlesSingleCharacterTokens) {
    Lexer lexer("+ - * / % ( ) { } [ ] , . ;");
    std::vector<Token> tokens = lexer.scanTokens();

    std::vector<TokenType> expected = {
        TokenType::PLUS, TokenType::MINUS, TokenType::STAR, TokenType::SLASH, TokenType::PERCENT,
        TokenType::LEFT_PAREN, TokenType::RIGHT_PAREN, TokenType::LEFT_BRACE, TokenType::RIGHT_BRACE,
        TokenType::LEFT_BRACKET, TokenType::RIGHT_BRACKET, TokenType::COMMA, TokenType::DOT,
        TokenType::SEMICOLON, TokenType::END_OF_FILE
    };

    ASSERT_EQ(tokens.size(), expected.size());
    for(size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected[i]);
    }
}

TEST(LexerTest, HandlesMultiCharacterTokens) {
    Lexer lexer("! != = == > >= < <=");
    std::vector<Token> tokens = lexer.scanTokens();

    std::vector<TokenType> expected = {
        TokenType::BANG, TokenType::BANG_EQUAL, TokenType::EQUAL, TokenType::EQUAL_EQUAL,
        TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL,
        TokenType::END_OF_FILE
    };

    ASSERT_EQ(tokens.size(), expected.size());
    for(size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected[i]);
    }
}

TEST(LexerTest, HandlesComments) {
    Lexer lexer("// this is a comment\nlet x = 10; // another comment");
    std::vector<Token> tokens = lexer.scanTokens();

    std::vector<TokenType> expected = {
        TokenType::NEWLINE, TokenType::LET, TokenType::IDENTIFIER, TokenType::EQUAL,
        TokenType::NUMBER_INT, TokenType::SEMICOLON, TokenType::END_OF_FILE
    };

    ASSERT_EQ(tokens.size(), expected.size());
    for(size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected[i]);
    }
}

TEST(LexerTest, HandlesMultiLineComments) {
    Lexer lexer("/* this is a \n multi-line comment */ let y = 20;");
    std::vector<Token> tokens = lexer.scanTokens();

    std::vector<TokenType> expected = {
        TokenType::LET, TokenType::IDENTIFIER, TokenType::EQUAL,
        TokenType::NUMBER_INT, TokenType::SEMICOLON, TokenType::END_OF_FILE
    };

    ASSERT_EQ(tokens.size(), expected.size());
    for(size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected[i]);
    }
}

TEST(LexerTest, HandlesIntegerAndFloatLiterals) {
    Lexer lexer("123 45.67");
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].type, TokenType::NUMBER_INT);
    EXPECT_EQ(std::get<int>(tokens[0].literal), 123);
    EXPECT_EQ(tokens[1].type, TokenType::NUMBER_FLOAT);
    EXPECT_EQ(std::get<double>(tokens[1].literal), 45.67);
    EXPECT_EQ(tokens[2].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, HandlesStringLiterals) {
    Lexer lexer("\"hello world\"");
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::STRING);
    EXPECT_EQ(std::get<std::string>(tokens[0].literal), "hello world");
    EXPECT_EQ(tokens[1].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, HandlesKeywordsAndIdentifiers) {
    Lexer lexer("let myVar = func");
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 5);
    EXPECT_EQ(tokens[0].type, TokenType::LET);
    EXPECT_EQ(tokens[1].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[1].lexeme, "myVar");
    EXPECT_EQ(tokens[2].type, TokenType::EQUAL);
    EXPECT_EQ(tokens[3].type, TokenType::FUNC);
    EXPECT_EQ(tokens[4].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, HandlesUnterminatedString) {
    Lexer lexer("\"hello");
    lexer.scanTokens();

    EXPECT_TRUE(lexer.hasError());

    auto errors = lexer.getErrors();
    ASSERT_EQ(errors.size(), 1);
    EXPECT_EQ(errors[0].line, 1);
    EXPECT_EQ(errors[0].message, "Unterminated string.");
}

TEST(LexerTest, HandlesUnexpectedCharacter) {
    Lexer lexer("@");
    lexer.scanTokens();

    EXPECT_TRUE(lexer.hasError());

    auto errors = lexer.getErrors();
    ASSERT_EQ(errors.size(), 1);
    EXPECT_EQ(errors[0].line, 1);
    EXPECT_EQ(errors[0].message, "Unexpected character.");
}
