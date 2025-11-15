#include <gtest/gtest.h>
#include "lexer.hpp"
#include <vector>

using namespace nota;

struct TokenInfo {
    TokenType type;
    std::string lexeme;
};

void checkTokens(Lexer& lexer, const std::vector<TokenInfo>& expectedTokens) {
    for (const auto& expectedToken : expectedTokens) {
        Token token = lexer.scanToken();
        EXPECT_EQ(token.type, expectedToken.type);
        EXPECT_EQ(token.lexeme, expectedToken.lexeme);
    }
    Token token = lexer.scanToken();
    EXPECT_EQ(token.type, TokenType::END_OF_FILE);
}

TEST(LexerTest, SingleCharacterTokens) {
    std::string source = "(){}[],.-+;*/%:";
    Lexer lexer(source);
    std::vector<TokenInfo> expectedTokens = {
        {TokenType::LEFT_PAREN, "("}, {TokenType::RIGHT_PAREN, ")"},
        {TokenType::LEFT_BRACE, "{"}, {TokenType::RIGHT_BRACE, "}"},
        {TokenType::LEFT_BRACKET, "["}, {TokenType::RIGHT_BRACKET, "]"},
        {TokenType::COMMA, ","}, {TokenType::DOT, "."},
        {TokenType::MINUS, "-"}, {TokenType::PLUS, "+"},
        {TokenType::SEMICOLON, ";"}, {TokenType::STAR, "*"},
        {TokenType::SLASH, "/"}, {TokenType::PERCENT, "%"},
        {TokenType::COLON, ":"},
    };
    checkTokens(lexer, expectedTokens);
}

TEST(LexerTest, DoubleCharacterTokens) {
    std::string source = "!= == <= >= += -= *= /= %= && || << >>";
    Lexer lexer(source);
    std::vector<TokenInfo> expectedTokens = {
        {TokenType::BANG_EQUAL, "!="}, {TokenType::EQUAL_EQUAL, "=="},
        {TokenType::LESS_EQUAL, "<="}, {TokenType::GREATER_EQUAL, ">="},
        {TokenType::PLUS_EQUAL, "+="}, {TokenType::MINUS_EQUAL, "-="},
        {TokenType::STAR_EQUAL, "*="}, {TokenType::SLASH_EQUAL, "/="},
        {TokenType::PERCENT_EQUAL, "%="}, {TokenType::AMPERSAND_AMPERSAND, "&&"},
        {TokenType::PIPE_PIPE, "||"}, {TokenType::LEFT_SHIFT, "<<"},
        {TokenType::RIGHT_SHIFT, ">>"},
    };
    checkTokens(lexer, expectedTokens);
}

TEST(LexerTest, Literals) {
    std::string source = "\"hello\" 123 123.456 identifier_name";
    Lexer lexer(source);
    std::vector<TokenInfo> expectedTokens = {
        {TokenType::STRING, "hello"},
        {TokenType::NUMBER, "123"},
        {TokenType::NUMBER, "123.456"},
        {TokenType::IDENTIFIER, "identifier_name"},
    };
    checkTokens(lexer, expectedTokens);
}

TEST(LexerTest, Keywords) {
    std::string source = "let mut true false if else while for do func class return super this import as package match end";
    Lexer lexer(source);
    std::vector<TokenInfo> expectedTokens = {
        {TokenType::LET, "let"}, {TokenType::MUT, "mut"},
        {TokenType::TRUE, "true"}, {TokenType::FALSE, "false"},
        {TokenType::IF, "if"}, {TokenType::ELSE, "else"},
        {TokenType::WHILE, "while"}, {TokenType::FOR, "for"},
        {TokenType::DO, "do"}, {TokenType::FUNC, "func"},
        {TokenType::CLASS, "class"}, {TokenType::RETURN, "return"},
        {TokenType::SUPER, "super"}, {TokenType::THIS, "this"},
        {TokenType::IMPORT, "import"}, {TokenType::AS, "as"},
        {TokenType::PACKAGE, "package"}, {TokenType::MATCH, "match"},
        {TokenType::END, "end"},
    };
    checkTokens(lexer, expectedTokens);
}

TEST(LexerTest, Comments) {
    std::string source = "# this is a comment\nlet a = 10";
    Lexer lexer(source);
    std::vector<TokenInfo> expectedTokens = {
        {TokenType::LET, "let"},
        {TokenType::IDENTIFIER, "a"},
        {TokenType::EQUAL, "="},
        {TokenType::NUMBER, "10"},
    };
    checkTokens(lexer, expectedTokens);
}

TEST(LexerTest, UnterminatedString) {
    std::string source = "\"hello";
    Lexer lexer(source);
    Token token = lexer.scanToken();
    EXPECT_EQ(token.type, TokenType::ERROR);
    EXPECT_EQ(token.lexeme, "Unterminated string.");
}
