#include <gtest/gtest.h>
#include "Parser.h"
#include "Lexer.h"

TEST(ParserTest, LetStatement) {
    std::string source = "let x = 123";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    Program program = parser.parse();

    ASSERT_EQ(program.size(), 1);

    auto* stmt = dynamic_cast<VarDeclStmt*>(program[0].get());
    ASSERT_NE(stmt, nullptr);
    ASSERT_EQ(stmt->name.text, "x");

    auto* expr = dynamic_cast<NumberExpr*>(stmt->initializer.get());
    ASSERT_NE(expr, nullptr);
    ASSERT_EQ(expr->value.text, "123");
}

TEST(ParserTest, LetStatementWithBinaryExpression) {
    std::string source = "let x = 1 + 2";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    Program program = parser.parse();

    ASSERT_EQ(program.size(), 1);

    auto* stmt = dynamic_cast<VarDeclStmt*>(program[0].get());
    ASSERT_NE(stmt, nullptr);
    ASSERT_EQ(stmt->name.text, "x");

    auto* bin_expr = dynamic_cast<BinaryExpr*>(stmt->initializer.get());
    ASSERT_NE(bin_expr, nullptr);
    ASSERT_EQ(bin_expr->op.type, TokenType::Plus);

    auto* left_expr = dynamic_cast<NumberExpr*>(bin_expr->left.get());
    ASSERT_NE(left_expr, nullptr);
    ASSERT_EQ(left_expr->value.text, "1");

    auto* right_expr = dynamic_cast<NumberExpr*>(bin_expr->right.get());
    ASSERT_NE(right_expr, nullptr);
    ASSERT_EQ(right_expr->value.text, "2");
}
