#include <gtest/gtest.h>
#include "parser/Parser.h"
#include "lexer/Lexer.h"

TEST(ParserTest, ParseSingleComponent) {
    std::string source = "App {}";
    Lexer lexer(source);
    Parser parser(lexer);
    RootNode root = parser.parse();

    ASSERT_NE(root.root_component, nullptr);
    EXPECT_EQ(root.root_component->type.text, "App");
    EXPECT_TRUE(root.root_component->properties.empty());
    EXPECT_TRUE(root.root_component->children.empty());
    EXPECT_TRUE(parser.errors().empty());
}

TEST(ParserTest, ParseComponentWithProperties) {
    std::string source = "App { width: 100; height: \"100%\"; }";
    Lexer lexer(source);
    Parser parser(lexer);
    RootNode root = parser.parse();

    ASSERT_NE(root.root_component, nullptr);
    EXPECT_EQ(root.root_component->type.text, "App");
    ASSERT_EQ(root.root_component->properties.size(), 2);

    // Property 1: width
    EXPECT_EQ(root.root_component->properties[0].name.text, "width");
    ASSERT_TRUE(std::holds_alternative<LiteralNode>(root.root_component->properties[0].value));
    auto& width_value = std::get<LiteralNode>(root.root_component->properties[0].value);
    ASSERT_TRUE(std::holds_alternative<double>(width_value.value));
    EXPECT_EQ(std::get<double>(width_value.value), 100);

    // Property 2: height
    EXPECT_EQ(root.root_component->properties[1].name.text, "height");
    ASSERT_TRUE(std::holds_alternative<LiteralNode>(root.root_component->properties[1].value));
    auto& height_value = std::get<LiteralNode>(root.root_component->properties[1].value);
    ASSERT_TRUE(std::holds_alternative<std::string>(height_value.value));
    EXPECT_EQ(std::get<std::string>(height_value.value), "\"100%\"");

    EXPECT_TRUE(parser.errors().empty());
}

TEST(ParserTest, ParseNestedComponents) {
    std::string source = "App { Row {} }";
    Lexer lexer(source);
    Parser parser(lexer);
    RootNode root = parser.parse();

    ASSERT_NE(root.root_component, nullptr);
    EXPECT_EQ(root.root_component->type.text, "App");
    ASSERT_EQ(root.root_component->children.size(), 1);

    auto& child = root.root_component->children[0];
    EXPECT_EQ(child->type.text, "Row");
    EXPECT_TRUE(child->properties.empty());
    EXPECT_TRUE(child->children.empty());

    EXPECT_TRUE(parser.errors().empty());
}

TEST(ParserTest, InvalidSyntaxShouldReportError) {
    std::string source = "App { width: ; }";
    Lexer lexer(source);
    Parser parser(lexer);
    parser.parse();

    EXPECT_FALSE(parser.errors().empty());
}
