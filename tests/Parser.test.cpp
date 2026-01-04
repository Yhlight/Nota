#include <gtest/gtest.h>
#include "Lexer.h"
#include "Parser.h"
#include "AST/Stmt.h"
#include <vector>
#include <memory>

TEST(ParserTest, ParsesSimpleComponent) {
    std::string source = "App {}";
    nota::Lexer lexer(source);
    std::vector<nota::Token> tokens = lexer.scan_tokens();

    nota::Parser parser(tokens);
    std::vector<std::unique_ptr<nota::ast::Stmt>> ast = parser.parse();

    ASSERT_EQ(ast.size(), 1);

    auto* component_stmt = dynamic_cast<nota::ast::ComponentStmt*>(ast[0].get());
    ASSERT_NE(component_stmt, nullptr);
    EXPECT_EQ(component_stmt->name.lexeme, "App");
    EXPECT_TRUE(component_stmt->body.empty());
}

TEST(ParserTest, ParsesComponentWithNumericProperty) {
    std::string source = "App { width: 100 }";
    nota::Lexer lexer(source);
    std::vector<nota::Token> tokens = lexer.scan_tokens();

    nota::Parser parser(tokens);
    std::vector<std::unique_ptr<nota::ast::Stmt>> ast = parser.parse();

    ASSERT_EQ(ast.size(), 1);

    auto* component_stmt = dynamic_cast<nota::ast::ComponentStmt*>(ast[0].get());
    ASSERT_NE(component_stmt, nullptr);
    EXPECT_EQ(component_stmt->name.lexeme, "App");

    ASSERT_EQ(component_stmt->body.size(), 1);
    auto* property_stmt = dynamic_cast<nota::ast::PropertyStmt*>(component_stmt->body[0].get());
    ASSERT_NE(property_stmt, nullptr);
    EXPECT_EQ(property_stmt->name.lexeme, "width");

    auto* literal_expr = dynamic_cast<nota::ast::LiteralExpr*>(property_stmt->value.get());
    ASSERT_NE(literal_expr, nullptr);
    EXPECT_EQ(literal_expr->value.type, nota::TokenType::NUMBER);
    EXPECT_EQ(literal_expr->value.lexeme, "100");
}
