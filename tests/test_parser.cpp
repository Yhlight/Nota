#include <gtest/gtest.h>
#include "parser.h"
#include "lexer.h"

TEST(ParserTest, ParseApp) {
    nota::Lexer lexer("App { width: 100 }");
    auto tokens = lexer.tokenize();
    nota::Parser parser(tokens);
    auto app = parser.parse();

    ASSERT_NE(app, nullptr);
    EXPECT_EQ(app->type, "App");
    ASSERT_EQ(app->properties.size(), 1);
    EXPECT_EQ(app->properties[0]->name, "width");
}

TEST(ParserTest, ParseNested) {
    nota::Lexer lexer("App { Rect { width: 50% } }");
    auto tokens = lexer.tokenize();
    nota::Parser parser(tokens);
    auto app = parser.parse();

    ASSERT_EQ(app->children.size(), 1);
    auto rect = app->children[0];
    EXPECT_EQ(rect->type, "Rect");
    ASSERT_EQ(rect->properties.size(), 1);

    auto val = std::dynamic_pointer_cast<nota::Literal>(rect->properties[0]->value);
    ASSERT_NE(val, nullptr);
    EXPECT_EQ(std::get<std::string>(val->value), "50%");
}
