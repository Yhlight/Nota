#include <gtest/gtest.h>
#include "Lexer.h"
#include "Parser.h"

TEST(ParserTest, BasicComponent) {
    std::string input = "App { width: 100% }";
    Lexer lexer(input);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto root = parser.parse();

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->type, "App");
    EXPECT_EQ(root->properties.size(), 1);
    EXPECT_EQ(root->properties[0].name, "width");
    EXPECT_EQ(root->properties[0].value, "100%");
}

TEST(ParserTest, NestedComponent) {
    std::string input = "App { Rect { color: red } }";
    Lexer lexer(input);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto root = parser.parse();

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->type, "App");
    EXPECT_EQ(root->children.size(), 1);
    EXPECT_EQ(root->children[0]->type, "Rect");
    EXPECT_EQ(root->children[0]->properties.size(), 1);
    EXPECT_EQ(root->children[0]->properties[0].name, "color");
    EXPECT_EQ(root->children[0]->properties[0].value, "red");
}

TEST(ParserTest, MultipleProperties) {
    std::string input = "Rect { width: 100; height: 100; color: #fff }";
    Lexer lexer(input);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto root = parser.parse();

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->properties.size(), 3);
    EXPECT_EQ(root->properties[0].name, "width");
    EXPECT_EQ(root->properties[0].value, "100");
    EXPECT_EQ(root->properties[2].name, "color");
    EXPECT_EQ(root->properties[2].value, "#fff");
}
