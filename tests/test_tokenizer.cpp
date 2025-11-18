#include <gtest/gtest.h>
#include "Tokenizer.h"

TEST(TokenizerTest, TokenizesPunctuators) {
    std::string source = "+ - * / % == != > >= < <= ! & | ^ ~ << >> = += -= *= /= %= && || ++ -- ( ) { } [ ] , . : ; =>";
    Tokenizer tokenizer(source);
    std::vector<Token> tokens = tokenizer.tokenize();

    std::vector<TokenType> expected_types = {
        TokenType::PLUS, TokenType::MINUS, TokenType::STAR, TokenType::SLASH, TokenType::PERCENT,
        TokenType::EQ, TokenType::NEQ, TokenType::GT, TokenType::GTE, TokenType::LT, TokenType::LTE,
        TokenType::NOT, TokenType::AMP, TokenType::PIPE, TokenType::CARET, TokenType::TILDE,
        TokenType::LSHIFT, TokenType::RSHIFT, TokenType::ASSIGN, TokenType::PLUS_ASSIGN,
        TokenType::MINUS_ASSIGN, TokenType::STAR_ASSIGN, TokenType::SLASH_ASSIGN, TokenType::PERCENT_ASSIGN,
        TokenType::AND, TokenType::OR, TokenType::INC, TokenType::DEC, TokenType::LPAREN, TokenType::RPAREN,
        TokenType::LBRACE, TokenType::RBRACE, TokenType::LBRACKET, TokenType::RBRACKET, TokenType::COMMA,
        TokenType::DOT, TokenType::COLON, TokenType::SEMICOLON, TokenType::ARROW, TokenType::END_OF_FILE
    };

    ASSERT_EQ(tokens.size(), expected_types.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected_types[i]);
    }
}

TEST(TokenizerTest, TokenizesKeywords) {
    std::string source = "let mut if else end match while for do func class return import package as none";
    Tokenizer tokenizer(source);
    std::vector<Token> tokens = tokenizer.tokenize();

    std::vector<TokenType> expected_types = {
        TokenType::LET, TokenType::MUT, TokenType::IF, TokenType::ELSE, TokenType::END,
        TokenType::MATCH, TokenType::WHILE, TokenType::FOR, TokenType::DO, TokenType::FUNC,
        TokenType::CLASS, TokenType::RETURN, TokenType::IMPORT, TokenType::PACKAGE, TokenType::AS,
        TokenType::NONE, TokenType::END_OF_FILE
    };

    ASSERT_EQ(tokens.size(), expected_types.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected_types[i]);
    }
}

TEST(TokenizerTest, TokenizesLiterals) {
    std::string source = "my_var 123 3.14 \"hello\" true false";
    Tokenizer tokenizer(source);
    std::vector<Token> tokens = tokenizer.tokenize();

    std::vector<TokenType> expected_types = {
        TokenType::IDENTIFIER, TokenType::INTEGER, TokenType::FLOAT,
        TokenType::STRING, TokenType::BOOL, TokenType::BOOL, TokenType::END_OF_FILE
    };

    ASSERT_EQ(tokens.size(), expected_types.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected_types[i]);
    }
}

TEST(TokenizerTest, SkipsWhitespaceAndComments) {
    std::string source = R"(
        // This is a comment.
        let x = 10 // Another comment.
        /* This is a
           multi-line comment. */
        let y = 20
    )";
    Tokenizer tokenizer(source);
    std::vector<Token> tokens = tokenizer.tokenize();

    std::vector<TokenType> expected_types = {
        TokenType::NEWLINE,
        TokenType::NEWLINE,
        TokenType::LET, TokenType::IDENTIFIER, TokenType::ASSIGN, TokenType::INTEGER, TokenType::NEWLINE,
        TokenType::NEWLINE,
        TokenType::LET, TokenType::IDENTIFIER, TokenType::ASSIGN, TokenType::INTEGER, TokenType::NEWLINE,
        TokenType::END_OF_FILE
    };

    ASSERT_EQ(tokens.size(), expected_types.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected_types[i]);
    }
}
