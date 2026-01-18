#include <gtest/gtest.h>
#include "lexer.h"

TEST(LexerTest, BasicTokens) {
    std::string source = "App { width: 100 }";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 7); // App, {, width, :, 100, }, EOF
    EXPECT_EQ(tokens[0].type, TokenType::KEYWORD_APP);
    EXPECT_EQ(tokens[1].type, TokenType::LBRACE);
    EXPECT_EQ(tokens[2].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[2].value, "width");
    EXPECT_EQ(tokens[3].type, TokenType::COLON);
    EXPECT_EQ(tokens[4].type, TokenType::NUMBER_LITERAL);
    EXPECT_EQ(tokens[4].value, "100");
    EXPECT_EQ(tokens[5].type, TokenType::RBRACE);
}

TEST(LexerTest, Keywords) {
    std::string source = "Item Row Col Rect Text property import export";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    EXPECT_EQ(tokens[0].type, TokenType::KEYWORD_ITEM);
    EXPECT_EQ(tokens[1].type, TokenType::KEYWORD_ROW);
    EXPECT_EQ(tokens[2].type, TokenType::KEYWORD_COL);
    EXPECT_EQ(tokens[3].type, TokenType::KEYWORD_RECT);
    EXPECT_EQ(tokens[4].type, TokenType::KEYWORD_TEXT);
    EXPECT_EQ(tokens[5].type, TokenType::KEYWORD_PROPERTY);
    EXPECT_EQ(tokens[6].type, TokenType::KEYWORD_IMPORT);
    EXPECT_EQ(tokens[7].type, TokenType::KEYWORD_EXPORT);
}

TEST(LexerTest, Literals) {
    std::string source = "\"string\" 123 45.67 #FFFFFF";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    EXPECT_EQ(tokens[0].type, TokenType::STRING_LITERAL);
    EXPECT_EQ(tokens[0].value, "string");
    EXPECT_EQ(tokens[1].type, TokenType::NUMBER_LITERAL);
    EXPECT_EQ(tokens[1].value, "123");
    EXPECT_EQ(tokens[2].type, TokenType::NUMBER_LITERAL);
    EXPECT_EQ(tokens[2].value, "45.67");
    EXPECT_EQ(tokens[3].type, TokenType::HEX_COLOR_LITERAL);
    EXPECT_EQ(tokens[3].value, "#FFFFFF");
}

TEST(LexerTest, Comments) {
    std::string source = "App // line comment\n{ /* block \n comment */ }";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 4); // App, {, }, EOF
    EXPECT_EQ(tokens[0].type, TokenType::KEYWORD_APP);
    EXPECT_EQ(tokens[1].type, TokenType::LBRACE);
    EXPECT_EQ(tokens[2].type, TokenType::RBRACE);
}

TEST(LexerTest, OperatorsAndSymbols) {
    std::string source = ". : ; [ ] ( ) + - * / = ,";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    EXPECT_EQ(tokens[0].type, TokenType::DOT);
    EXPECT_EQ(tokens[1].type, TokenType::COLON);
    EXPECT_EQ(tokens[2].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[3].type, TokenType::LBRACKET);
    EXPECT_EQ(tokens[4].type, TokenType::RBRACKET);
    EXPECT_EQ(tokens[5].type, TokenType::LPAREN);
    EXPECT_EQ(tokens[6].type, TokenType::RPAREN);
    EXPECT_EQ(tokens[7].type, TokenType::PLUS);
    EXPECT_EQ(tokens[8].type, TokenType::MINUS);
    EXPECT_EQ(tokens[9].type, TokenType::STAR);
    EXPECT_EQ(tokens[10].type, TokenType::SLASH);
    EXPECT_EQ(tokens[11].type, TokenType::EQUAL);
    EXPECT_EQ(tokens[12].type, TokenType::COMMA);
}
