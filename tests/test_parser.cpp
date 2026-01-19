#include <gtest/gtest.h>
#include "Lexer.h"
#include "Parser.h"
#include "ComponentRegistry.h"
#include "AST.h"

class ParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        ComponentRegistry::instance().clear();
    }
};

TEST_F(ParserTest, BasicComponent) {
    std::string input = "App { width: 100% }";
    Lexer lexer(input);
    Parser parser(lexer.tokenize());
    auto nodes = parser.parseAll();

    ASSERT_EQ(nodes.size(), 1);

    auto* app = dynamic_cast<ComponentNode*>(nodes[0].get());
    ASSERT_NE(app, nullptr);

    EXPECT_EQ(app->type, "App");
    EXPECT_EQ(app->properties.size(), 1);
    EXPECT_EQ(app->properties[0].name, "width");
}

TEST_F(ParserTest, ExpressionArithmetic) {
    std::string input = "Rect { width: 10 + 20 }";
    Lexer lexer(input);
    Parser parser(lexer.tokenize());
    ASSERT_NO_THROW(parser.parseAll());
}

TEST_F(ParserTest, ExpressionPrecedence) {
    std::string input = "Rect { width: 10 + 20 * 5 }"; // Should be 110
    Lexer lexer(input);
    Parser parser(lexer.tokenize());
    ASSERT_NO_THROW(parser.parseAll());
}

TEST_F(ParserTest, MultiTokenValue) {
    std::string input = "Rect { border: 1px solid black }";
    Lexer lexer(input);
    Parser parser(lexer.tokenize());
    ASSERT_NO_THROW(parser.parseAll());
}

TEST_F(ParserTest, PropertyAccess) {
    std::string input = "Rect { width: parent.width }";
    Lexer lexer(input);
    Parser parser(lexer.tokenize());
    ASSERT_NO_THROW(parser.parseAll());
}
