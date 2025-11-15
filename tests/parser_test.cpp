#include <gtest/gtest.h>
#include "../src/parser.hpp"
#include "../src/lexer.hpp"
#include <vector>

TEST(ParserTest, ParsesLetStatement) {
    std::string source = "let x = 10";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    LetStmt* let_stmt = dynamic_cast<LetStmt*>(statements[0].get());
    ASSERT_NE(let_stmt, nullptr);
    EXPECT_EQ(let_stmt->name.lexeme, "x");

    LiteralExpr* literal_expr = dynamic_cast<LiteralExpr*>(let_stmt->initializer.get());
    ASSERT_NE(literal_expr, nullptr);
    EXPECT_EQ(literal_expr->value.lexeme, "10");
}

TEST(ParserTest, ParsesMutStatement) {
    std::string source = "mut y = \"hello\"";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    MutStmt* mut_stmt = dynamic_cast<MutStmt*>(statements[0].get());
    ASSERT_NE(mut_stmt, nullptr);
    EXPECT_EQ(mut_stmt->name.lexeme, "y");

    LiteralExpr* literal_expr = dynamic_cast<LiteralExpr*>(mut_stmt->initializer.get());
    ASSERT_NE(literal_expr, nullptr);
    EXPECT_EQ(literal_expr->value.lexeme, "\"hello\"");
}

TEST(ParserTest, ParsesLiteralExpression) {
    std::string source = "123";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    ExpressionStmt* expr_stmt = dynamic_cast<ExpressionStmt*>(statements[0].get());
    ASSERT_NE(expr_stmt, nullptr);

    LiteralExpr* literal_expr = dynamic_cast<LiteralExpr*>(expr_stmt->expression.get());
    ASSERT_NE(literal_expr, nullptr);
    EXPECT_EQ(literal_expr->value.lexeme, "123");
}

TEST(ParserTest, ParsesBinaryExpression) {
    std::string source = "1 + 2";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    ExpressionStmt* expr_stmt = dynamic_cast<ExpressionStmt*>(statements[0].get());
    ASSERT_NE(expr_stmt, nullptr);

    BinaryExpr* binary_expr = dynamic_cast<BinaryExpr*>(expr_stmt->expression.get());
    ASSERT_NE(binary_expr, nullptr);

    LiteralExpr* left = dynamic_cast<LiteralExpr*>(binary_expr->left.get());
    ASSERT_NE(left, nullptr);
    EXPECT_EQ(left->value.lexeme, "1");

    EXPECT_EQ(binary_expr->op.type, TokenType::PLUS);

    LiteralExpr* right = dynamic_cast<LiteralExpr*>(binary_expr->right.get());
    ASSERT_NE(right, nullptr);
    EXPECT_EQ(right->value.lexeme, "2");
}
