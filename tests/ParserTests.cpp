#include <gtest/gtest.h>
#include "Parser.hpp"
#include "Lexer.hpp"
#include "AST.hpp"

using namespace Nota;

TEST(ParserTest, SimpleComponent) {
    std::string source = R"(
        App {
            width: 100%;
        }
    )";
    Lexer lexer(source);
    auto tokens = lexer.ScanTokens();

    Parser parser(tokens);
    auto ast = parser.Parse();

    ASSERT_NE(ast, nullptr);
    ASSERT_EQ(ast->type.type, TokenType::KW_APP);
    ASSERT_EQ(ast->properties.size(), 1);
    EXPECT_EQ(ast->properties[0]->name.lexeme, "width");

    auto literal = dynamic_cast<LiteralExpr*>(ast->properties[0]->value.get());
    ASSERT_NE(literal, nullptr);
    EXPECT_EQ(literal->type, TokenType::PERCENTAGE);
}

TEST(ParserTest, NestedComponents) {
    std::string source = R"(
        Row {
            Col {
                width: 50;
            }
        }
    )";
    Lexer lexer(source);
    auto tokens = lexer.ScanTokens();

    Parser parser(tokens);
    auto ast = parser.Parse();

    ASSERT_NE(ast, nullptr);
    ASSERT_EQ(ast->type.type, TokenType::KW_ROW);
    ASSERT_EQ(ast->children.size(), 1);

    auto child = ast->children[0].get();
    ASSERT_EQ(child->type.type, TokenType::KW_COL);
    ASSERT_EQ(child->properties.size(), 1);
    EXPECT_EQ(child->properties[0]->name.lexeme, "width");
}

TEST(ParserTest, MissingClosingBrace) {
    std::string source = R"(
        App {
            width: 100%;
    )";
    Lexer lexer(source);
    auto tokens = lexer.ScanTokens();

    Parser parser(tokens);
    auto ast = parser.Parse();

    EXPECT_EQ(ast, nullptr);
    ASSERT_FALSE(parser.Errors().empty());
    EXPECT_NE(parser.Errors()[0].find("Expect '}' after component block."), std::string::npos);
}

TEST(ParserTest, InvalidProperty) {
    std::string source = R"(
        App {
            width: ;
        }
    )";
    Lexer lexer(source);
    auto tokens = lexer.ScanTokens();

    Parser parser(tokens);
    auto ast = parser.Parse();

    EXPECT_EQ(ast, nullptr);
    ASSERT_FALSE(parser.Errors().empty());
    EXPECT_NE(parser.Errors()[0].find("Expect expression."), std::string::npos);
}

TEST(ParserTest, MultipleProperties) {
    std::string source = R"(
        Rect {
            height: 200;
            color: #ffffff;
            radius: 8;
        }
    )";
    Lexer lexer(source);
    auto tokens = lexer.ScanTokens();

    Parser parser(tokens);
    auto ast = parser.Parse();

    ASSERT_NE(ast, nullptr);
    ASSERT_EQ(ast->type.type, TokenType::KW_RECT);
    ASSERT_EQ(ast->properties.size(), 3);
    EXPECT_EQ(ast->properties[0]->name.lexeme, "height");
    EXPECT_EQ(ast->properties[1]->name.lexeme, "color");
    EXPECT_EQ(ast->properties[2]->name.lexeme, "radius");
}
