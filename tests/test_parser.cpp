#include <gtest/gtest.h>
#include "parser.h"

TEST(ParserTest, BasicComponent) {
    // App { }
    std::vector<Token> tokens = {
        {TokenType::KEYWORD_APP, "App", 1, 1},
        {TokenType::LBRACE, "{", 1, 5},
        {TokenType::RBRACE, "}", 1, 7},
        {TokenType::EOF_TOKEN, "", 1, 8}
    };

    Parser parser(tokens);
    auto root = parser.parse();

    EXPECT_EQ(root->type, "App");
    EXPECT_EQ(root->children.size(), 0);
}

TEST(ParserTest, Properties) {
    // Rect { width: 100 }
    std::vector<Token> tokens = {
        {TokenType::KEYWORD_RECT, "Rect", 1, 1},
        {TokenType::LBRACE, "{", 1, 6},
        {TokenType::IDENTIFIER, "width", 1, 8},
        {TokenType::COLON, ":", 1, 13},
        {TokenType::NUMBER_LITERAL, "100", 1, 15},
        {TokenType::RBRACE, "}", 1, 19},
        {TokenType::EOF_TOKEN, "", 1, 20}
    };

    Parser parser(tokens);
    auto root = parser.parse();

    EXPECT_EQ(root->type, "Rect");
    ASSERT_EQ(root->children.size(), 1);

    auto prop = std::dynamic_pointer_cast<PropertyNode>(root->children[0]);
    ASSERT_NE(prop, nullptr);
    EXPECT_EQ(prop->name, "width");

    auto val = std::dynamic_pointer_cast<ValueNode>(prop->value);
    ASSERT_NE(val, nullptr);
    EXPECT_EQ(val->token.value, "100");
}

TEST(ParserTest, NestedComponents) {
    // App { Row { } }
    std::vector<Token> tokens = {
        {TokenType::KEYWORD_APP, "App", 1, 1},
        {TokenType::LBRACE, "{", 1, 5},
            {TokenType::KEYWORD_ROW, "Row", 2, 5},
            {TokenType::LBRACE, "{", 2, 9},
            {TokenType::RBRACE, "}", 2, 11},
        {TokenType::RBRACE, "}", 3, 1},
        {TokenType::EOF_TOKEN, "", 3, 2}
    };

    Parser parser(tokens);
    auto root = parser.parse();

    EXPECT_EQ(root->type, "App");
    ASSERT_EQ(root->children.size(), 1);

    auto child = std::dynamic_pointer_cast<ComponentNode>(root->children[0]);
    ASSERT_NE(child, nullptr);
    EXPECT_EQ(child->type, "Row");
}

TEST(ParserTest, ItemDefinition) {
    // Item Box { }
    std::vector<Token> tokens = {
        {TokenType::KEYWORD_ITEM, "Item", 1, 1},
        {TokenType::IDENTIFIER, "Box", 1, 6},
        {TokenType::LBRACE, "{", 1, 10},
        {TokenType::RBRACE, "}", 1, 12},
        {TokenType::EOF_TOKEN, "", 1, 13}
    };

    Parser parser(tokens);
    auto root = parser.parse();

    EXPECT_EQ(root->type, "Item");
    EXPECT_EQ(root->name, "Box");
}
