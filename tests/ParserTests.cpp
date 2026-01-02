#include <gtest/gtest.h>
#include "../src/Lexer.hpp"
#include "../src/Parser.hpp"
#include "../src/AST.hpp"

TEST(ParserTest, ParsesSimpleComponent) {
    std::string source = "App { width: 100 }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::unique_ptr<Component> root = parser.parse();

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->name.lexeme, "App");
    ASSERT_EQ(root->properties.size(), 1);
    EXPECT_EQ(root->properties[0]->name.lexeme, "width");

    Literal* value = dynamic_cast<Literal*>(root->properties[0]->value.get());
    ASSERT_NE(value, nullptr);
    EXPECT_EQ(value->value.lexeme, "100");
}

TEST(ParserTest, ParsesNestedComponents) {
    std::string source = R"(
        App {
            Row {
                Text { content: "hello" }
            }
        }
    )";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::unique_ptr<Component> root = parser.parse();

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->name.lexeme, "App");
    ASSERT_EQ(root->children.size(), 1);

    Component* row = root->children[0].get();
    EXPECT_EQ(row->name.lexeme, "Row");
    ASSERT_EQ(row->children.size(), 1);

    Component* text = row->children[0].get();
    EXPECT_EQ(text->name.lexeme, "Text");
    ASSERT_EQ(text->properties.size(), 1);
    EXPECT_EQ(text->properties[0]->name.lexeme, "content");
}

TEST(ParserTest, ParsesMultipleProperties) {
    std::string source = "Rect { width: 100; height: 200; color: #fff }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::unique_ptr<Component> root = parser.parse();

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->name.lexeme, "Rect");
    ASSERT_EQ(root->properties.size(), 3);
    EXPECT_EQ(root->properties[0]->name.lexeme, "width");
    EXPECT_EQ(root->properties[1]->name.lexeme, "height");
    EXPECT_EQ(root->properties[2]->name.lexeme, "color");
}
