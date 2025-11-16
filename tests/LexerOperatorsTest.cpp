#include "gtest/gtest.h"
#include "Lexer.h"
#include "Token.h"

using namespace nota;

TEST(LexerOperatorsTest, AllOperators) {
    std::string source = "+ - * / % == != > >= < <= ! & | ^ ~ << >> += -= *= /= %= && || ++ --";
    Lexer lexer(source);

    TokenType expected[] = {
        TokenType::Plus, TokenType::Minus, TokenType::Star, TokenType::Slash, TokenType::Percent,
        TokenType::EqualEqual, TokenType::BangEqual, TokenType::Greater, TokenType::GreaterEqual,
        TokenType::Less, TokenType::LessEqual, TokenType::Bang, TokenType::Ampersand, TokenType::Pipe,
        TokenType::Caret, TokenType::Tilde, TokenType::LessLess, TokenType::GreaterGreater,
        TokenType::PlusEqual, TokenType::MinusEqual, TokenType::StarEqual, TokenType::SlashEqual,
        TokenType::PercentEqual, TokenType::AmpersandAmpersand, TokenType::PipePipe,
        TokenType::PlusPlus, TokenType::MinusMinus, TokenType::Eof
    };

    for (auto expected_type : expected) {
        Token token = lexer.next_token();
        EXPECT_EQ(token.type, expected_type);
    }
}

TEST(LexerKeywordsTest, AllKeywords) {
    std::string source = "let mut if else match while for do func class import as package return true false";
    Lexer lexer(source);

    TokenType expected[] = {
        TokenType::Let, TokenType::Mut, TokenType::If, TokenType::Else, TokenType::Match,
        TokenType::While, TokenType::For, TokenType::Do, TokenType::Func, TokenType::Class,
        TokenType::Import, TokenType::As, TokenType::Package, TokenType::Return,
        TokenType::True, TokenType::False, TokenType::Eof
    };

    for (auto expected_type : expected) {
        Token token = lexer.next_token();
        EXPECT_EQ(token.type, expected_type);
    }
}

TEST(LexerStringTest, SimpleString) {
    std::string source = "\"hello world\"";
    Lexer lexer(source);
    Token token = lexer.next_token();
    EXPECT_EQ(token.type, TokenType::String);
    EXPECT_EQ(token.lexeme, "\"hello world\"");
}
