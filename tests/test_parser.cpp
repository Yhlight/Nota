#include <gtest/gtest.h>
#include "Lexer.hpp"
#include "Parser.hpp"
#include "AST.hpp"

TEST(ParserTest, ParseSimpleComponent) {
    std::string source = "App { }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::unique_ptr<Program> program = parser.parse();

    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->statements.size(), 1);

    auto* stmt = program->statements[0].get();
    ASSERT_NE(stmt, nullptr);

    auto* component = dynamic_cast<ComponentStmt*>(stmt);
    ASSERT_NE(component, nullptr);
    EXPECT_EQ(component->type.lexeme, "App");
    EXPECT_EQ(component->body.size(), 0);
}

TEST(ParserTest, ParseComponentWithPropertiesAndNesting) {
    std::string source = R"(
        App {
            width: 1024
            title: "My App"

            Row {
                spacing: 10
            }
        }
    )";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::unique_ptr<Program> program = parser.parse();

    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->statements.size(), 1);

    // Check the root App component
    auto* root_stmt = program->statements[0].get();
    ASSERT_NE(root_stmt, nullptr);
    auto* root_component = dynamic_cast<ComponentStmt*>(root_stmt);
    ASSERT_NE(root_component, nullptr);
    EXPECT_EQ(root_component->type.lexeme, "App");
    ASSERT_EQ(root_component->body.size(), 3); // width, title, Row

    // Check the 'width' property
    auto* width_stmt = root_component->body[0].get();
    ASSERT_NE(width_stmt, nullptr);
    auto* width_prop = dynamic_cast<PropertyStmt*>(width_stmt);
    ASSERT_NE(width_prop, nullptr);
    EXPECT_EQ(width_prop->name.lexeme, "width");
    auto* width_value = dynamic_cast<LiteralExpr*>(width_prop->value.get());
    ASSERT_NE(width_value, nullptr);
    EXPECT_EQ(width_value->value.lexeme, "1024");

    // Check the 'title' property
    auto* title_stmt = root_component->body[1].get();
    ASSERT_NE(title_stmt, nullptr);
    auto* title_prop = dynamic_cast<PropertyStmt*>(title_stmt);
    ASSERT_NE(title_prop, nullptr);
    EXPECT_EQ(title_prop->name.lexeme, "title");
    auto* title_value = dynamic_cast<LiteralExpr*>(title_prop->value.get());
    ASSERT_NE(title_value, nullptr);
    EXPECT_EQ(title_value->value.lexeme, "\"My App\"");

    // Check the nested Row component
    auto* row_stmt = root_component->body[2].get();
    ASSERT_NE(row_stmt, nullptr);
    auto* row_component = dynamic_cast<ComponentStmt*>(row_stmt);
    ASSERT_NE(row_component, nullptr);
    EXPECT_EQ(row_component->type.lexeme, "Row");
    ASSERT_EQ(row_component->body.size(), 1);

    // Check the 'spacing' property of the nested Row
    auto* spacing_stmt = row_component->body[0].get();
    ASSERT_NE(spacing_stmt, nullptr);
    auto* spacing_prop = dynamic_cast<PropertyStmt*>(spacing_stmt);
    ASSERT_NE(spacing_prop, nullptr);
    EXPECT_EQ(spacing_prop->name.lexeme, "spacing");
    auto* spacing_value = dynamic_cast<LiteralExpr*>(spacing_prop->value.get());
    ASSERT_NE(spacing_value, nullptr);
    EXPECT_EQ(spacing_value->value.lexeme, "10");
}
