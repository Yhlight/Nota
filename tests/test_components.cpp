#include <gtest/gtest.h>
#include "Lexer.h"
#include "Parser.h"
#include "ComponentRegistry.h"
#include "CodeGen.h"

class ComponentTest : public ::testing::Test {
protected:
    void SetUp() override {
        ComponentRegistry::instance().clear();
    }
};

TEST_F(ComponentTest, DefineAndInstantiate) {
    std::string input =
        "Item Box { width: 50; height: 50 } "
        "App { Box { x: 10 } }";

    Lexer lexer(input);
    Parser parser(lexer.tokenize());
    auto nodes = parser.parseAll();

    ASSERT_EQ(nodes.size(), 1);
    auto& app = nodes[0];
    EXPECT_EQ(app->type, "App");

    ASSERT_EQ(app->children.size(), 1);
    auto& box = app->children[0];

    // Box should have properties from definition + override
    // From definition: width, height
    // From instance: x
    // Total 3
    EXPECT_EQ(box->properties.size(), 3);

    // Check override/values
    bool foundWidth = false;
    bool foundX = false;

    // Need Evaluator to check values now
    Evaluator eval;

    for (const auto& p : box->properties) {
        if (p.name == "width" && eval.evaluate(*p.value) == "50") foundWidth = true;
        if (p.name == "x" && eval.evaluate(*p.value) == "10") foundX = true;
    }
    EXPECT_TRUE(foundWidth);
    EXPECT_TRUE(foundX);
}

TEST_F(ComponentTest, PropertyOverride) {
    std::string input =
        "Item Box { color: red } "
        "App { Box { color: blue } }";

    Lexer lexer(input);
    Parser parser(lexer.tokenize());
    auto nodes = parser.parseAll();

    ASSERT_EQ(nodes.size(), 1);
    auto& app = nodes[0];
    auto& box = app->children[0];

    // Should have only 1 color property, value blue
    int colorCount = 0;
    std::string colorVal;
    Evaluator eval;

    for (const auto& p : box->properties) {
        if (p.name == "color") {
            colorCount++;
            colorVal = eval.evaluate(*p.value);
        }
    }

    EXPECT_EQ(colorCount, 1);
    EXPECT_EQ(colorVal, "blue");
}

TEST_F(ComponentTest, ChildAppend) {
    std::string input =
        "Item Box { Rect { id: r1 } } "
        "App { Box { Rect { id: r2 } } }";

    Lexer lexer(input);
    Parser parser(lexer.tokenize());
    auto nodes = parser.parseAll();

    ASSERT_EQ(nodes.size(), 1);
    auto& app = nodes[0];
    auto& box = app->children[0];

    // Should have 2 children: r1 (from def) and r2 (appended)
    EXPECT_EQ(box->children.size(), 2);
}
