#include <gtest/gtest.h>
#include <Parser.hpp>
#include <Lexer.hpp>

TEST(ParserTest, ParseEmptyComponent) {
    std::string source = "App {}";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::unique_ptr<ComponentNode> root = parser.parse();

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->type.type, TokenType::APP);
    EXPECT_TRUE(root->properties.empty());
    EXPECT_TRUE(root->children.empty());
}

TEST(ParserTest, ParseComponentWithProperties) {
    std::string source = R"(
        App {
            width: 100
            title: "Hello"
        }
    )";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::unique_ptr<ComponentNode> root = parser.parse();

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->type.type, TokenType::APP);
    ASSERT_EQ(root->properties.size(), 2);

    EXPECT_EQ(root->properties[0]->key.lexeme, "width");
    EXPECT_EQ(root->properties[0]->value.lexeme, "100");

    EXPECT_EQ(root->properties[1]->key.lexeme, "title");
    EXPECT_EQ(root->properties[1]->value.lexeme, "\"Hello\"");

    EXPECT_TRUE(root->children.empty());
}

TEST(ParserTest, ParseNestedComponent) {
    std::string source = R"(
        App {
            Rect {}
        }
    )";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::unique_ptr<ComponentNode> root = parser.parse();

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->type.type, TokenType::APP);
    EXPECT_TRUE(root->properties.empty());

    ASSERT_EQ(root->children.size(), 1);
    EXPECT_EQ(root->children[0]->type.type, TokenType::RECT);
    EXPECT_TRUE(root->children[0]->properties.empty());
    EXPECT_TRUE(root->children[0]->children.empty());
}
