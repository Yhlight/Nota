#include <gtest/gtest.h>
#include "parser.h"
#include "lexer.h"

TEST(ParserTest, ParseApp) {
    nota::Lexer lexer("App { width: 100 }");
    auto tokens = lexer.tokenize();
    nota::Parser parser(tokens);
    auto program = parser.parse();

    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->components.size(), 1);
    auto app = program->components[0];

    EXPECT_EQ(app->type, "App");
    ASSERT_EQ(app->properties.size(), 1);
    EXPECT_EQ(app->properties[0]->name, "width");
}

TEST(ParserTest, ParseNested) {
    nota::Lexer lexer("App { Rect { width: 50% } }");
    auto tokens = lexer.tokenize();
    nota::Parser parser(tokens);
    auto program = parser.parse();

    ASSERT_EQ(program->components.size(), 1);
    auto app = program->components[0];

    ASSERT_EQ(app->children.size(), 1);
    auto rect = app->children[0];
    EXPECT_EQ(rect->type, "Rect");
    ASSERT_EQ(rect->properties.size(), 1);

    auto val = std::dynamic_pointer_cast<nota::Literal>(rect->properties[0]->value);
    ASSERT_NE(val, nullptr);
    EXPECT_EQ(std::get<std::string>(val->value), "50%");
}

TEST(ParserTest, ParseBinaryExpr) {
    nota::Lexer lexer("App { width: 100 + 50 * 2 }");
    auto tokens = lexer.tokenize();
    nota::Parser parser(tokens);
    auto program = parser.parse();

    ASSERT_EQ(program->components.size(), 1);
    auto app = program->components[0];

    // 100 + (50 * 2)
    auto prop = app->properties[0];
    auto bin = std::dynamic_pointer_cast<nota::BinaryExpression>(prop->value);
    ASSERT_NE(bin, nullptr);
    EXPECT_EQ(bin->op, "+");

    auto left = std::dynamic_pointer_cast<nota::Literal>(bin->left);
    EXPECT_EQ(std::get<int>(left->value), 100);

    auto right = std::dynamic_pointer_cast<nota::BinaryExpression>(bin->right);
    ASSERT_NE(right, nullptr);
    EXPECT_EQ(right->op, "*");
}

TEST(ParserTest, ParseMemberExpr) {
    nota::Lexer lexer("App { width: box.width }");
    auto tokens = lexer.tokenize();
    nota::Parser parser(tokens);
    auto program = parser.parse();

    ASSERT_EQ(program->components.size(), 1);
    auto app = program->components[0];

    auto prop = app->properties[0];
    auto mem = std::dynamic_pointer_cast<nota::MemberExpression>(prop->value);
    ASSERT_NE(mem, nullptr);
    EXPECT_EQ(mem->member, "width");

    auto obj = std::dynamic_pointer_cast<nota::Identifier>(mem->object);
    EXPECT_EQ(obj->name, "box");
}
