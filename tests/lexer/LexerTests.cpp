#include <gtest/gtest.h>
#include "lexer/Lexer.h"
#include <vector>

struct TokenExpectation {
    TokenType type;
    std::string text;
};

void run_lexer_test(const std::string& source, const std::vector<TokenExpectation>& expectations) {
    Lexer lexer(source);
    for (const auto& expected : expectations) {
        Token token = lexer.next_token();
        EXPECT_EQ(token.type, expected.type);
        EXPECT_EQ(token.text, expected.text);
    }
    Token token = lexer.next_token();
    EXPECT_EQ(token.type, TokenType::END_OF_FILE);
}

TEST(LexerTest, SingleCharacterTokens) {
    std::string source = "{}:; / *"; // Separated / and * to avoid comment ambiguity
    std::vector<TokenExpectation> expectations = {
        {TokenType::LEFT_BRACE, "{"},
        {TokenType::RIGHT_BRACE, "}"},
        {TokenType::COLON, ":"},
        {TokenType::SEMICOLON, ";"},
        {TokenType::SLASH, "/"},
        {TokenType::STAR, "*"},
    };
    run_lexer_test(source, expectations);
}

TEST(LexerTest, PercentageLiteral) {
    std::string source = "100%";
    std::vector<TokenExpectation> expectations = {
        {TokenType::PERCENTAGE, "100%"},
    };
    run_lexer_test(source, expectations);
}

TEST(LexerTest, IdentifierAndKeywords) {
    std::string source = "App Item width";
    std::vector<TokenExpectation> expectations = {
        {TokenType::IDENTIFIER, "App"},
        {TokenType::ITEM, "Item"},
        {TokenType::IDENTIFIER, "width"},
    };
    run_lexer_test(source, expectations);
}

TEST(LexerTest, StringLiteral) {
    std::string source = "\"hello world\"";
    std::vector<TokenExpectation> expectations = {
        {TokenType::STRING, "\"hello world\""},
    };
    run_lexer_test(source, expectations);
}

TEST(LexerTest, NumberLiteral) {
    std::string source = "123 45.67";
    std::vector<TokenExpectation> expectations = {
        {TokenType::NUMBER, "123"},
        {TokenType::NUMBER, "45.67"},
    };
    run_lexer_test(source, expectations);
}

TEST(LexerTest, WhitespaceAndComments) {
    std::string source = R"(
        // This is a single-line comment.
        App {
            /* This is a
               multi-line comment. */
            width: 100
        }
    )";
    std::vector<TokenExpectation> expectations = {
        {TokenType::IDENTIFIER, "App"},
        {TokenType::LEFT_BRACE, "{"},
        {TokenType::IDENTIFIER, "width"},
        {TokenType::COLON, ":"},
        {TokenType::NUMBER, "100"},
        {TokenType::RIGHT_BRACE, "}"},
    };
    run_lexer_test(source, expectations);
}

TEST(LexerTest, UnterminatedString) {
    std::string source = "\"hello";
    Lexer lexer(source);
    Token token = lexer.next_token();
    EXPECT_EQ(token.type, TokenType::UNEXPECTED);
}

TEST(LexerTest, UnterminatedComment) {
    std::string source = "/* hello";
    Lexer lexer(source);
    Token token = lexer.next_token();
    EXPECT_EQ(token.type, TokenType::UNEXPECTED);
    EXPECT_EQ(token.text, "Unterminated comment.");
}
