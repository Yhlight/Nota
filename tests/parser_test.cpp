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

TEST(ParserTest, OperatorPrecedence) {
    std::string source = "let x = 1 + 2 * 3";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    Program program = parser.parse();

    ASSERT_EQ(program.size(), 1);

    auto* stmt = dynamic_cast<VarDeclStmt*>(program[0].get());
    ASSERT_NE(stmt, nullptr);

    auto* add_expr = dynamic_cast<BinaryExpr*>(stmt->initializer.get());
    ASSERT_NE(add_expr, nullptr);
    ASSERT_EQ(add_expr->op.type, TokenType::Plus);

    auto* left_num = dynamic_cast<NumberExpr*>(add_expr->left.get());
    ASSERT_NE(left_num, nullptr);
    ASSERT_EQ(left_num->value.text, "1");

    auto* mul_expr = dynamic_cast<BinaryExpr*>(add_expr->right.get());
    ASSERT_NE(mul_expr, nullptr);
    ASSERT_EQ(mul_expr->op.type, TokenType::Asterisk);

    auto* mid_num = dynamic_cast<NumberExpr*>(mul_expr->left.get());
    ASSERT_NE(mid_num, nullptr);
    ASSERT_EQ(mid_num->value.text, "2");

    auto* right_num = dynamic_cast<NumberExpr*>(mul_expr->right.get());
    ASSERT_NE(right_num, nullptr);
    ASSERT_EQ(right_num->value.text, "3");
}
