#include <gtest/gtest.h>
#include "Lexer.hpp"
#include "Token.hpp"
#include <vector>

using namespace Nota;

// Helper function to compare a vector of tokens against an expected list
void assertTokensEqual(const std::vector<Token>& tokens, const std::vector<TokenType>& expected) {
    ASSERT_EQ(tokens.size(), expected.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected[i]) << "Token " << i << " mismatch. Expected "
                                               << TokenTypeToString(expected[i]) << ", got "
                                               << TokenTypeToString(tokens[i].type);
    }
}

TEST(LexerTest, SimpleComponent) {
    std::string source = R"(
        App {
            width: 100%;
            height: 60;
            color: #f0f0f0;
        }
    )";

    Lexer lexer(source);
    auto tokens = lexer.ScanTokens();

    std::vector<TokenType> expected = {
        TokenType::KW_APP, TokenType::LEFT_BRACE,
        TokenType::IDENTIFIER, TokenType::COLON, TokenType::PERCENTAGE, TokenType::SEMICOLON,
        TokenType::IDENTIFIER, TokenType::COLON, TokenType::NUMBER, TokenType::SEMICOLON,
        TokenType::IDENTIFIER, TokenType::COLON, TokenType::COLOR_HEX, TokenType::SEMICOLON,
        TokenType::RIGHT_BRACE,
        TokenType::END_OF_FILE
    };

    assertTokensEqual(tokens, expected);
}

TEST(LexerTest, AllTokenTypes) {
    std::string source = R"(
        // Test all tokens
        {} [] : . = /
        "string" 123 45.6% #abc
        App Row Col Rect Text Item this parent
        /* multi-line
           comment */
        identifier_name
    )";

    Lexer lexer(source);
    auto tokens = lexer.ScanTokens();

    std::vector<TokenType> expected = {
        TokenType::LEFT_BRACE, TokenType::RIGHT_BRACE,
        TokenType::LEFT_BRACKET, TokenType::RIGHT_BRACKET,
        TokenType::COLON, TokenType::DOT, TokenType::EQUAL, TokenType::SLASH,
        TokenType::STRING, TokenType::NUMBER, TokenType::PERCENTAGE, TokenType::COLOR_HEX,
        TokenType::KW_APP, TokenType::KW_ROW, TokenType::KW_COL, TokenType::KW_RECT,
        TokenType::KW_TEXT, TokenType::KW_ITEM, TokenType::KW_THIS, TokenType::KW_PARENT,
        TokenType::IDENTIFIER,
        TokenType::END_OF_FILE
    };

    assertTokensEqual(tokens, expected);
    EXPECT_EQ(tokens[8].lexeme, "string");
    EXPECT_EQ(tokens[20].lexeme, "identifier_name");
}

TEST(LexerTest, Comments) {
    std::string source = R"(
        // This is a single-line comment.
        Rect {
            /* This is a
               multi-line comment. */
            color: #fff; // Another comment
        }
    )";

    Lexer lexer(source);
    auto tokens = lexer.ScanTokens();

    std::vector<TokenType> expected = {
        TokenType::KW_RECT, TokenType::LEFT_BRACE,
        TokenType::IDENTIFIER, TokenType::COLON, TokenType::COLOR_HEX, TokenType::SEMICOLON,
        TokenType::RIGHT_BRACE,
        TokenType::END_OF_FILE
    };

    assertTokensEqual(tokens, expected);
}

TEST(LexerTest, NestedStructure) {
    std::string source = R"(
        App {
            Row {
                Text {
                    text: "Hello, Nota!"
                }
            }
        }
    )";

    Lexer lexer(source);
    auto tokens = lexer.ScanTokens();

    std::vector<TokenType> expected = {
        TokenType::KW_APP, TokenType::LEFT_BRACE,
        TokenType::KW_ROW, TokenType::LEFT_BRACE,
        TokenType::KW_TEXT, TokenType::LEFT_BRACE,
        TokenType::IDENTIFIER, TokenType::COLON, TokenType::STRING,
        TokenType::RIGHT_BRACE,
        TokenType::RIGHT_BRACE,
        TokenType::RIGHT_BRACE,
        TokenType::END_OF_FILE
    };

    assertTokensEqual(tokens, expected);
}

TEST(LexerTest, UnterminatedString) {
    std::string source = R"( "hello )";
    Lexer lexer(source);
    auto tokens = lexer.ScanTokens();

    std::vector<TokenType> expected = {
        TokenType::UNKNOWN,
        TokenType::END_OF_FILE
    };

    assertTokensEqual(tokens, expected);
}
