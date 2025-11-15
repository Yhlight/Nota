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

TEST(ParserTest, ParsesIfStatement) {
    std::string source = "if true\nlet x = 1\nend";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    IfStmt* if_stmt = dynamic_cast<IfStmt*>(statements[0].get());
    ASSERT_NE(if_stmt, nullptr);

    LiteralExpr* condition = dynamic_cast<LiteralExpr*>(if_stmt->condition.get());
    ASSERT_NE(condition, nullptr);
    EXPECT_EQ(condition->value.type, TokenType::TRUE);

    BlockStmt* then_branch = dynamic_cast<BlockStmt*>(if_stmt->then_branch.get());
    ASSERT_NE(then_branch, nullptr);
    ASSERT_EQ(then_branch->statements.size(), 1);

    EXPECT_EQ(if_stmt->else_branch, nullptr);
}

TEST(ParserTest, ParsesIfElseStatement) {
    std::string source = "if false\nlet x = 1\nelse\nlet y = 2\nend";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    IfStmt* if_stmt = dynamic_cast<IfStmt*>(statements[0].get());
    ASSERT_NE(if_stmt, nullptr);

    LiteralExpr* condition = dynamic_cast<LiteralExpr*>(if_stmt->condition.get());
    ASSERT_NE(condition, nullptr);
    EXPECT_EQ(condition->value.type, TokenType::FALSE);

    BlockStmt* then_branch = dynamic_cast<BlockStmt*>(if_stmt->then_branch.get());
    ASSERT_NE(then_branch, nullptr);
    ASSERT_EQ(then_branch->statements.size(), 1);

    BlockStmt* else_branch = dynamic_cast<BlockStmt*>(if_stmt->else_branch.get());
    ASSERT_NE(else_branch, nullptr);
    ASSERT_EQ(else_branch->statements.size(), 1);
}

TEST(ParserTest, ParsesWhileStatement) {
    std::string source = "while true\nlet x = 1\nend";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    WhileStmt* while_stmt = dynamic_cast<WhileStmt*>(statements[0].get());
    ASSERT_NE(while_stmt, nullptr);

    LiteralExpr* condition = dynamic_cast<LiteralExpr*>(while_stmt->condition.get());
    ASSERT_NE(condition, nullptr);
    EXPECT_EQ(condition->value.type, TokenType::TRUE);

    BlockStmt* body = dynamic_cast<BlockStmt*>(while_stmt->body.get());
    ASSERT_NE(body, nullptr);
    ASSERT_EQ(body->statements.size(), 1);
}

TEST(ParserTest, ParsesFunctionDeclaration) {
    std::string source = "func my_func(a, b)\nreturn a + b\nend";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    FunctionStmt* func_stmt = dynamic_cast<FunctionStmt*>(statements[0].get());
    ASSERT_NE(func_stmt, nullptr);
    EXPECT_EQ(func_stmt->name.lexeme, "my_func");
    ASSERT_EQ(func_stmt->params.size(), 2);
    EXPECT_EQ(func_stmt->params[0].lexeme, "a");
    EXPECT_EQ(func_stmt->params[1].lexeme, "b");
    ASSERT_EQ(func_stmt->body.size(), 1);
}

TEST(ParserTest, ParsesCallExpression) {
    std::string source = "my_func(1, 2)";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    ExpressionStmt* expr_stmt = dynamic_cast<ExpressionStmt*>(statements[0].get());
    ASSERT_NE(expr_stmt, nullptr);

    CallExpr* call_expr = dynamic_cast<CallExpr*>(expr_stmt->expression.get());
    ASSERT_NE(call_expr, nullptr);

    VariableExpr* callee = dynamic_cast<VariableExpr*>(call_expr->callee.get());
    ASSERT_NE(callee, nullptr);
    EXPECT_EQ(callee->name.lexeme, "my_func");

    ASSERT_EQ(call_expr->arguments.size(), 2);
}

TEST(ParserTest, ParsesReturnStatement) {
    std::string source = "return 1";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    ReturnStmt* return_stmt = dynamic_cast<ReturnStmt*>(statements[0].get());
    ASSERT_NE(return_stmt, nullptr);

    LiteralExpr* value = dynamic_cast<LiteralExpr*>(return_stmt->value.get());
    ASSERT_NE(value, nullptr);
    EXPECT_EQ(value->value.lexeme, "1");
}
