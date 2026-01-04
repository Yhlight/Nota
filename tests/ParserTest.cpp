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
