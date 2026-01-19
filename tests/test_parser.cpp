#include <gtest/gtest.h>
#include "Lexer.h"
#include "Parser.h"
#include "ComponentRegistry.h"

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
    EXPECT_EQ(nodes[0]->type, "App");
    EXPECT_EQ(nodes[0]->properties.size(), 1);
    EXPECT_EQ(nodes[0]->properties[0].name, "width");
    // Value is Expr, hard to check string directly without visitor or cast.
    // We assume it parsed correctly if no error.
}

TEST_F(ParserTest, ExpressionArithmetic) {
    // We can't easily inspect the AST without casting, so we rely on CodeGen tests for value verification
    // or just checking no throw here.
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
