#include "gtest/gtest.h"
#include "Lexer.h"
#include "Token.h"

using namespace nota;

TEST(LexerTest, SingleLineComment) {
    std::string source = "// this is a comment\n";
    Lexer lexer(source);
    Token token = lexer.next_token();
    EXPECT_EQ(token.type, TokenType::Comment);
    EXPECT_EQ(token.lexeme, "// this is a comment");
}

TEST(LexerTest, MultiLineComment) {
    std::string source = "/* this is a multi-line comment */";
    Lexer lexer(source);
    Token token = lexer.next_token();
    EXPECT_EQ(token.type, TokenType::Comment);
    EXPECT_EQ(token.lexeme, "/* this is a multi-line comment */");
}

TEST(LexerTest, MultiLineCommentWithNewlines) {
    std::string source = "/*\nthis is a\nmulti-line comment\n*/";
    Lexer lexer(source);
    Token token = lexer.next_token();
    EXPECT_EQ(token.type, TokenType::Comment);
    EXPECT_EQ(token.lexeme, "/*\nthis is a\nmulti-line comment\n*/");
}

TEST(LexerTest, EmptyComment) {
    std::string source = "//\n";
    Lexer lexer(source);
    Token token = lexer.next_token();
    EXPECT_EQ(token.type, TokenType::Comment);
    EXPECT_EQ(token.lexeme, "//");
}

TEST(LexerTest, CommentAtEndOfFile) {
    std::string source = "// this is a comment";
    Lexer lexer(source);
    Token token = lexer.next_token();
    EXPECT_EQ(token.type, TokenType::Comment);
    EXPECT_EQ(token.lexeme, "// this is a comment");
}
