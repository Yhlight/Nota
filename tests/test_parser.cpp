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

    ASSERT_EQ(root->statements.size(), 1);
    auto comp = std::dynamic_pointer_cast<ComponentNode>(root->statements[0]);
    ASSERT_NE(comp, nullptr);
    EXPECT_EQ(comp->type, "App");
    EXPECT_EQ(comp->children.size(), 0);
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

    ASSERT_EQ(root->statements.size(), 1);
    auto comp = std::dynamic_pointer_cast<ComponentNode>(root->statements[0]);
    ASSERT_NE(comp, nullptr);

    EXPECT_EQ(comp->type, "Rect");
    ASSERT_EQ(comp->children.size(), 1);

    auto prop = std::dynamic_pointer_cast<PropertyNode>(comp->children[0]);
    ASSERT_NE(prop, nullptr);
    EXPECT_EQ(prop->name, "width");

    auto val = std::dynamic_pointer_cast<LiteralNode>(prop->value);
    ASSERT_NE(val, nullptr);
    EXPECT_EQ(val->token.value, "100");
}

TEST(ParserTest, Imports) {
    // import "ui.nota"; App {}
    std::vector<Token> tokens = {
        {TokenType::KEYWORD_IMPORT, "import", 1, 1},
        {TokenType::STRING_LITERAL, "ui.nota", 1, 8},
        {TokenType::SEMICOLON, ";", 1, 17},
        {TokenType::KEYWORD_APP, "App", 1, 19},
        {TokenType::LBRACE, "{", 1, 23},
        {TokenType::RBRACE, "}", 1, 25},
        {TokenType::EOF_TOKEN, "", 1, 26}
    };

    Parser parser(tokens);
    auto root = parser.parse();

    ASSERT_EQ(root->statements.size(), 2);

    auto importNode = std::dynamic_pointer_cast<ImportNode>(root->statements[0]);
    ASSERT_NE(importNode, nullptr);
    EXPECT_EQ(importNode->path, "ui.nota");

    auto appNode = std::dynamic_pointer_cast<ComponentNode>(root->statements[1]);
    ASSERT_NE(appNode, nullptr);
    EXPECT_EQ(appNode->type, "App");
}

TEST(ParserTest, ImportWithAlias) {
    // import UI as ui
    std::vector<Token> tokens = {
        {TokenType::KEYWORD_IMPORT, "import", 1, 1},
        {TokenType::IDENTIFIER, "UI", 1, 8},
        {TokenType::IDENTIFIER, "as", 1, 11},
        {TokenType::IDENTIFIER, "ui", 1, 14},
        {TokenType::EOF_TOKEN, "", 1, 16}
    };

    Parser parser(tokens);
    auto root = parser.parse();

    ASSERT_EQ(root->statements.size(), 1);
    auto importNode = std::dynamic_pointer_cast<ImportNode>(root->statements[0]);
    ASSERT_NE(importNode, nullptr);
    EXPECT_EQ(importNode->path, "UI");
    EXPECT_EQ(importNode->alias, "ui");
}

TEST(ParserTest, BinaryExpression) {
    // Rect { width: 100 + 20 }
    std::vector<Token> tokens = {
        {TokenType::KEYWORD_RECT, "Rect", 1, 1},
        {TokenType::LBRACE, "{", 1, 6},
        {TokenType::IDENTIFIER, "width", 1, 8},
        {TokenType::COLON, ":", 1, 13},
        {TokenType::NUMBER_LITERAL, "100", 1, 15},
        {TokenType::PLUS, "+", 1, 19},
        {TokenType::NUMBER_LITERAL, "20", 1, 21},
        {TokenType::RBRACE, "}", 1, 23},
        {TokenType::EOF_TOKEN, "", 1, 24}
    };

    Parser parser(tokens);
    auto root = parser.parse();

    auto comp = std::dynamic_pointer_cast<ComponentNode>(root->statements[0]);
    auto prop = std::dynamic_pointer_cast<PropertyNode>(comp->children[0]);
    auto expr = std::dynamic_pointer_cast<BinaryExpressionNode>(prop->value);

    ASSERT_NE(expr, nullptr);
    EXPECT_EQ(expr->op.type, TokenType::PLUS);
}

TEST(ParserTest, StructDefinition) {
    // Struct Point { double x; }
    std::vector<Token> tokens = {
        {TokenType::KEYWORD_STRUCT, "Struct", 1, 1},
        {TokenType::IDENTIFIER, "Point", 1, 8},
        {TokenType::LBRACE, "{", 1, 14},
        {TokenType::IDENTIFIER, "double", 1, 16},
        {TokenType::IDENTIFIER, "x", 1, 23},
        {TokenType::SEMICOLON, ";", 1, 24},
        {TokenType::RBRACE, "}", 1, 26},
        {TokenType::EOF_TOKEN, "", 1, 27}
    };

    Parser parser(tokens);
    auto root = parser.parse();

    ASSERT_EQ(root->statements.size(), 1);
    auto strct = std::dynamic_pointer_cast<StructDefinitionNode>(root->statements[0]);
    ASSERT_NE(strct, nullptr);
    EXPECT_EQ(strct->name, "Point");
    ASSERT_EQ(strct->fields.size(), 1);
    EXPECT_EQ(strct->fields[0].type, "double");
    EXPECT_EQ(strct->fields[0].name, "x");
}
