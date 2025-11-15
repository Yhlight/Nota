#include <gtest/gtest.h>
#include "../src/lexer.hpp"

using namespace nota;

TEST(LexerTest, SingleCharacterTokens) {
    std::string source = "(){}[],.-+*%!~^:";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();

    std::vector<TokenType> expected_types = {
        TokenType::LEFT_PAREN, TokenType::RIGHT_PAREN,
        TokenType::LEFT_BRACE, TokenType::RIGHT_BRACE,
        TokenType::LEFT_BRACKET, TokenType::RIGHT_BRACKET,
        TokenType::COMMA, TokenType::DOT, TokenType::MINUS,
        TokenType::PLUS, TokenType::STAR, TokenType::PERCENT,
        TokenType::BANG, TokenType::BITWISE_NOT, TokenType::BITWISE_XOR,
        TokenType::COLON, TokenType::EOF_TOKEN
    };

    ASSERT_EQ(tokens.size(), expected_types.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected_types[i]);
    }
}

TEST(LexerTest, OperatorTokens) {
    std::string source = "!= == <= >= += -= *= /= %= && || & | << >> = =>";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();

    std::vector<TokenType> expected_types = {
        TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL,
        TokenType::LESS_EQUAL, TokenType::GREATER_EQUAL,
        TokenType::PLUS_EQUAL, TokenType::MINUS_EQUAL,
        TokenType::STAR_EQUAL, TokenType::SLASH_EQUAL,
        TokenType::PERCENT_EQUAL, TokenType::AND, TokenType::OR,
        TokenType::BITWISE_AND, TokenType::BITWISE_OR,
        TokenType::LEFT_SHIFT, TokenType::RIGHT_SHIFT,
        TokenType::EQUAL, TokenType::ARROW, TokenType::EOF_TOKEN
    };

    ASSERT_EQ(tokens.size(), expected_types.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected_types[i]);
    }
}

TEST(LexerTest, StringLiteral) {
    std::string source = "\"hello world\"";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();

    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::STRING);
    EXPECT_EQ(tokens[0].lexeme, source);
}

TEST(LexerTest, NumberLiterals) {
    std::string source = "123 45.67";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();

    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[0].lexeme, "123");
    EXPECT_EQ(tokens[1].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[1].lexeme, "45.67");
}

TEST(LexerTest, IdentifiersAndKeywords) {
    std::string source = "let mut x = 10; class myClass";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();

    std::vector<TokenType> expected_types = {
        TokenType::LET, TokenType::MUT, TokenType::IDENTIFIER,
        TokenType::EQUAL, TokenType::NUMBER, TokenType::SEMICOLON, TokenType::CLASS,
        TokenType::IDENTIFIER, TokenType::EOF_TOKEN
    };

    // Note: Semicolon is not part of the language and will be ignored.
    ASSERT_EQ(tokens.size(), expected_types.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected_types[i]);
    }
}
