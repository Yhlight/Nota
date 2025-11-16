#include "gtest/gtest.h"
#include "Lexer.h"
#include "Token.h"

using namespace nota;

TEST(LexerBasicsTest, LetKeyword) {
    std::string source = "let";
    Lexer lexer(source);
    Token token = lexer.next_token();
    EXPECT_EQ(token.type, TokenType::Let);
    EXPECT_EQ(token.lexeme, "let");
}

TEST(LexerBasicsTest, MutKeyword) {
    std::string source = "mut";
    Lexer lexer(source);
    Token token = lexer.next_token();
    EXPECT_EQ(token.type, TokenType::Mut);
    EXPECT_EQ(token.lexeme, "mut");
}

TEST(LexerBasicsTest, Identifier) {
    std::string source = "my_variable";
    Lexer lexer(source);
    Token token = lexer.next_token();
    EXPECT_EQ(token.type, TokenType::Identifier);
    EXPECT_EQ(token.lexeme, "my_variable");
}

TEST(LexerBasicsTest, Newline) {
    std::string source = "\n";
    Lexer lexer(source);
    Token token = lexer.next_token();
    EXPECT_EQ(token.type, TokenType::Newline);
}

TEST(LexerBasicsTest, PlusOperator) {
    std::string source = "+";
    Lexer lexer(source);
    Token token = lexer.next_token();
    EXPECT_EQ(token.type, TokenType::Plus);
    EXPECT_EQ(token.lexeme, "+");
}

TEST(LexerBasicsTest, StatementContinuation) {
    std::string source = "let a = 10 +\n20";
    Lexer lexer(source);

    Token token1 = lexer.next_token();
    EXPECT_EQ(token1.type, TokenType::Let);

    Token token2 = lexer.next_token();
    EXPECT_EQ(token2.type, TokenType::Identifier);
    EXPECT_EQ(token2.lexeme, "a");

    // Skip the equals and 10 for now
    lexer.next_token();
    lexer.next_token();

    Token token3 = lexer.next_token();
    EXPECT_EQ(token3.type, TokenType::Plus);

    Token token4 = lexer.next_token();
    EXPECT_EQ(token4.type, TokenType::Newline);
}
