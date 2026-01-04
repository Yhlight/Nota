#include <gtest/gtest.h>
#include "Lexer.h"
#include "Parser.h"
#include "AST/Stmt.h"
#include "AST/Expr.h"
#include <vector>
#include <memory>

TEST(ParserTest, SimpleComponent) {
    std::string source = "App { width: 100 }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);

    // Check the root ComponentStmt
    auto* root_stmt = dynamic_cast<ComponentStmt*>(statements[0].get());
    ASSERT_NE(root_stmt, nullptr);
    EXPECT_EQ(root_stmt->name.lexeme, "App");

    // Check the body of the ComponentStmt
    ASSERT_EQ(root_stmt->body.size(), 1);
    auto* prop_stmt = dynamic_cast<PropertyStmt*>(root_stmt->body[0].get());
    ASSERT_NE(prop_stmt, nullptr);
    EXPECT_EQ(prop_stmt->name.lexeme, "width");

    // Check the value of the PropertyStmt
    auto* literal_expr = dynamic_cast<LiteralExpr*>(prop_stmt->value.get());
    ASSERT_NE(literal_expr, nullptr);
    EXPECT_EQ(std::any_cast<double>(literal_expr->value), 100.0);
}

TEST(ParserTest, PropertyReference) {
    std::string source = "App { id: myApp width: myApp.width / 2 }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);

    auto* root_stmt = dynamic_cast<ComponentStmt*>(statements[0].get());
    ASSERT_NE(root_stmt, nullptr);
    ASSERT_EQ(root_stmt->body.size(), 2);

    // Check the 'width' property
    auto* width_prop = dynamic_cast<PropertyStmt*>(root_stmt->body[1].get());
    ASSERT_NE(width_prop, nullptr);

    // Check that the value is a BinaryExpr
    auto* binary_expr = dynamic_cast<BinaryExpr*>(width_prop->value.get());
    ASSERT_NE(binary_expr, nullptr);
    EXPECT_EQ(binary_expr->op.type, TokenType::SLASH);

    // Check the left side of the binary expression (the GetExpr)
    auto* get_expr = dynamic_cast<GetExpr*>(binary_expr->left.get());
    ASSERT_NE(get_expr, nullptr);
    EXPECT_EQ(get_expr->name.lexeme, "width");

    // Check the object of the GetExpr
    auto* get_obj = dynamic_cast<IdentifierExpr*>(get_expr->object.get());
    ASSERT_NE(get_obj, nullptr);
    EXPECT_EQ(get_obj->name.lexeme, "myApp");

    // Check the right side of the binary expression (the LiteralExpr)
    auto* literal_expr = dynamic_cast<LiteralExpr*>(binary_expr->right.get());
    ASSERT_NE(literal_expr, nullptr);
    EXPECT_EQ(std::any_cast<double>(literal_expr->value), 2.0);
}
