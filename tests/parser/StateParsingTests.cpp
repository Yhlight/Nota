#include "parser/Parser.h"
#include "lexer/Lexer.h"
#include <gtest/gtest.h>

TEST(ParserTest, ParseStateDeclaration) {
    std::string source = R"(
        App {
            state myState: "initial";
            state anotherState: 123;
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    RootNode root = parser.parse();

    ASSERT_TRUE(root.root_component);
    ASSERT_EQ(root.root_component->states.size(), 2);

    EXPECT_EQ(root.root_component->states[0].name.text, "myState");
    ASSERT_TRUE(std::holds_alternative<LiteralNode>(root.root_component->states[0].value));
    auto value1 = std::get<LiteralNode>(root.root_component->states[0].value);
    ASSERT_TRUE(std::holds_alternative<std::string>(value1.value));
    EXPECT_EQ(std::get<std::string>(value1.value), "\"initial\"");

    EXPECT_EQ(root.root_component->states[1].name.text, "anotherState");
    ASSERT_TRUE(std::holds_alternative<std::unique_ptr<Expression>>(root.root_component->states[1].value));
    auto expr = std::move(std::get<std::unique_ptr<Expression>>(root.root_component->states[1].value));
    ASSERT_TRUE(std::holds_alternative<LiteralNode>(expr->variant));
    auto value2 = std::get<LiteralNode>(expr->variant);
    ASSERT_TRUE(std::holds_alternative<double>(value2.value));
    EXPECT_EQ(std::get<double>(value2.value), 123);
}
