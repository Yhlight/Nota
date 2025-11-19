#include <gtest/gtest.h>
#include "lexer/Lexer.hpp"
#include <vector>

using namespace nota::core;
using namespace nota::lexer;

struct TokenInfo {
    TokenType type;
    std::string_view literal;
};

void ExpectTokens(const std::string& source, const std::vector<TokenInfo>& expected_tokens) {
    Lexer lexer(source);
    auto tokens = lexer.Tokenize();

    ASSERT_EQ(tokens.size(), expected_tokens.size() + 1);

    for (size_t i = 0; i < expected_tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected_tokens[i].type);
        EXPECT_EQ(tokens[i].literal, expected_tokens[i].literal);
    }

    EXPECT_EQ(tokens.back().type, TokenType::END_OF_FILE);
}

TEST(LexerTest, BasicOperators) {
    std::string source = "+-*/% = ! > < & | ^ ~ : .";
    std::vector<TokenInfo> expected = {
        {TokenType::PLUS, "+"}, {TokenType::MINUS, "-"}, {TokenType::ASTERISK, "*"}, {TokenType::SLASH, "/"},
        {TokenType::PERCENT, "%"}, {TokenType::ASSIGN, "="}, {TokenType::BANG, "!"}, {TokenType::GT, ">"},
        {TokenType::LT, "<"}, {TokenType::AMPERSAND, "&"}, {TokenType::PIPE, "|"}, {TokenType::CARET, "^"},
        {TokenType::TILDE, "~"}, {TokenType::COLON, ":"}, {TokenType::DOT, "."}
    };
    ExpectTokens(source, expected);
}

TEST(LexerTest, TwoCharOperators) {
    std::string source = "++ -- == != >= <= += -= *= /= %= && || => ::";
    std::vector<TokenInfo> expected = {
        {TokenType::PLUS_PLUS, "++"}, {TokenType::MINUS_MINUS, "--"}, {TokenType::EQ, "=="},
        {TokenType::NOT_EQ, "!="}, {TokenType::GT_EQ, ">="}, {TokenType::LT_EQ, "<="},
        {TokenType::PLUS_ASSIGN, "+="}, {TokenType::MINUS_ASSIGN, "-="}, {TokenType::ASTERISK_ASSIGN, "*="},
        {TokenType::SLASH_ASSIGN, "/="}, {TokenType::PERCENT_ASSIGN, "%="}, {TokenType::AND, "&&"},
        {TokenType::OR, "||"}, {TokenType::ARROW, "=>"}, {TokenType::DOUBLE_COLON, "::"}
    };
    ExpectTokens(source, expected);
}

TEST(LexerTest, Delimiters) {
    std::string source = "() {} [] ,";
    std::vector<TokenInfo> expected = {
        {TokenType::LPAREN, "("}, {TokenType::RPAREN, ")"}, {TokenType::LBRACE, "{"},
        {TokenType::RBRACE, "}"}, {TokenType::LBRACKET, "["}, {TokenType::RBRACKET, "]"},
        {TokenType::COMMA, ","}
    };
    ExpectTokens(source, expected);
}

TEST(LexerTest, Keywords) {
    std::string source = "let mut if else end match while for do func class import as package return this none true false";
    std::vector<TokenInfo> expected = {
        {TokenType::LET, "let"}, {TokenType::MUT, "mut"}, {TokenType::IF, "if"}, {TokenType::ELSE, "else"},
        {TokenType::END, "end"}, {TokenType::MATCH, "match"}, {TokenType::WHILE, "while"}, {TokenType::FOR, "for"},
        {TokenType::DO, "do"}, {TokenType::FUNC, "func"}, {TokenType::CLASS, "class"}, {TokenType::IMPORT, "import"},
        {TokenType::AS, "as"}, {TokenType::PACKAGE, "package"}, {TokenType::RETURN, "return"}, {TokenType::THIS, "this"},
        {TokenType::NONE, "none"}, {TokenType::TRUE, "true"}, {TokenType::FALSE, "false"}
    };
    ExpectTokens(source, expected);
}

TEST(LexerTest, Literals) {
    std::string source = "my_var 123 45.67 \"hello world\"";
    std::vector<TokenInfo> expected = {
        {TokenType::IDENTIFIER, "my_var"}, {TokenType::INTEGER, "123"}, {TokenType::FLOAT, "45.67"},
        {TokenType::STRING, "\"hello world\""}
    };
    ExpectTokens(source, expected);
}

TEST(LexerTest, Comments) {
    std::string source = "// this is a comment\nlet a = 10 /* another comment */";
    std::vector<TokenInfo> expected = {
        {TokenType::NEWLINE, "\n"}, {TokenType::LET, "let"}, {TokenType::IDENTIFIER, "a"},
        {TokenType::ASSIGN, "="}, {TokenType::INTEGER, "10"}
    };
    ExpectTokens(source, expected);
}

TEST(LexerTest, StatementContinuation) {
    std::string source = "let a = 10 +\n20";
    std::vector<TokenInfo> expected = {
        {TokenType::LET, "let"}, {TokenType::IDENTIFIER, "a"}, {TokenType::ASSIGN, "="},
        {TokenType::INTEGER, "10"}, {TokenType::PLUS, "+"}, {TokenType::NEWLINE, "\n"},
        {TokenType::INTEGER, "20"}
    };
    ExpectTokens(source, expected);
}
