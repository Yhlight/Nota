#include "gtest/gtest.h"
#include "Lexer.h"
#include "Parser.h"
#include "AST/AstPrinter.h"
#include <vector>

TEST(ParserTest, VarDeclInt) {
    std::string source = "let x = 5\n";
    nota::Lexer lexer(source);
    nota::Parser parser(lexer);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
    auto* var_decl = dynamic_cast<nota::ast::VarDeclStmt*>(stmts[0].get());
    ASSERT_NE(var_decl, nullptr);
    EXPECT_EQ(var_decl->name.lexeme, "x");
}

TEST(ParserTest, VarDeclFloat) {
    std::string source = "let pi = 3.14\n";
    nota::Lexer lexer(source);
    nota::Parser parser(lexer);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
    auto* var_decl = dynamic_cast<nota::ast::VarDeclStmt*>(stmts[0].get());
    ASSERT_NE(var_decl, nullptr);
    EXPECT_EQ(var_decl->name.lexeme, "pi");
}

TEST(ParserTest, VarDeclWithType) {
    std::string source = "let x: int = 5\n";
    nota::Lexer lexer(source);
    nota::Parser parser(lexer);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
    auto* var_decl = dynamic_cast<nota::ast::VarDeclStmt*>(stmts[0].get());
    ASSERT_NE(var_decl, nullptr);
    EXPECT_EQ(var_decl->name.lexeme, "x");
    ASSERT_NE(var_decl->type, nullptr);
    auto* base_type = dynamic_cast<nota::ast::BaseType*>(var_decl->type.get());
    ASSERT_NE(base_type, nullptr);
    EXPECT_EQ(base_type->name.lexeme, "int");
}

TEST(ParserTest, UnaryExpr) {
    std::string source = "let x = -5\n";
    nota::Lexer lexer(source);
    nota::Parser parser(lexer);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
    auto* var_decl = dynamic_cast<nota::ast::VarDeclStmt*>(stmts[0].get());
    ASSERT_NE(var_decl, nullptr);
    auto* unary_expr = dynamic_cast<nota::ast::UnaryExpr*>(var_decl->initializer.get());
    ASSERT_NE(unary_expr, nullptr);
    EXPECT_EQ(unary_expr->op.type, nota::TokenType::Minus);
}

TEST(ParserTest, BinaryExpr) {
    std::string source = "let x = 5 + 10\n";
    nota::Lexer lexer(source);
    nota::Parser parser(lexer);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
    auto* var_decl = dynamic_cast<nota::ast::VarDeclStmt*>(stmts[0].get());
    ASSERT_NE(var_decl, nullptr);
    auto* binary_expr = dynamic_cast<nota::ast::BinaryExpr*>(var_decl->initializer.get());
    ASSERT_NE(binary_expr, nullptr);
    EXPECT_EQ(binary_expr->op.type, nota::TokenType::Plus);
}

TEST(ParserTest, LogicalExpr) {
    std::string source = "let x = 5 > 10 || 2 < 4\n";
    nota::Lexer lexer(source);
    nota::Parser parser(lexer);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
}

TEST(ParserTest, IfStmt) {
    std::string source = "if x > 5\n let y = 10\nend\n";
    nota::Lexer lexer(source);
    nota::Parser parser(lexer);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
    auto* if_stmt = dynamic_cast<nota::ast::IfStmt*>(stmts[0].get());
    ASSERT_NE(if_stmt, nullptr);
    EXPECT_NE(if_stmt->then_branch, nullptr);
    EXPECT_EQ(if_stmt->else_branch, nullptr);
}

TEST(ParserTest, IfElseStmt) {
    std::string source = "if x > 5\n let y = 10\nelse\n let y = 20\nend\n";
    nota::Lexer lexer(source);
    nota::Parser parser(lexer);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
    auto* if_stmt = dynamic_cast<nota::ast::IfStmt*>(stmts[0].get());
    ASSERT_NE(if_stmt, nullptr);
    EXPECT_NE(if_stmt->then_branch, nullptr);
    EXPECT_NE(if_stmt->else_branch, nullptr);
    auto* else_block = dynamic_cast<nota::ast::BlockStmt*>(if_stmt->else_branch.get());
    ASSERT_NE(else_block, nullptr);
}

TEST(ParserTest, IfElseIfStmt) {
    std::string source = "if x > 5\n let y = 10\nelse if x < 5\n let y = 20\nend\n";
    nota::Lexer lexer(source);
    nota::Parser parser(lexer);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
    auto* if_stmt = dynamic_cast<nota::ast::IfStmt*>(stmts[0].get());
    ASSERT_NE(if_stmt, nullptr);
    EXPECT_NE(if_stmt->then_branch, nullptr);
    EXPECT_NE(if_stmt->else_branch, nullptr);
    auto* else_if_stmt = dynamic_cast<nota::ast::IfStmt*>(if_stmt->else_branch.get());
    ASSERT_NE(else_if_stmt, nullptr);
    EXPECT_NE(else_if_stmt->then_branch, nullptr);
    EXPECT_EQ(else_if_stmt->else_branch, nullptr);
}

TEST(ParserTest, IfElseIfElseStmt) {
    std::string source = "if x > 5\n let y = 10\nelse if x < 5\n let y = 20\nelse\n let y = 30\nend\n";
    nota::Lexer lexer(source);
    nota::Parser parser(lexer);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
    auto* if_stmt = dynamic_cast<nota::ast::IfStmt*>(stmts[0].get());
    ASSERT_NE(if_stmt, nullptr);
    EXPECT_NE(if_stmt->then_branch, nullptr);
    EXPECT_NE(if_stmt->else_branch, nullptr);
    auto* else_if_stmt = dynamic_cast<nota::ast::IfStmt*>(if_stmt->else_branch.get());
    ASSERT_NE(else_if_stmt, nullptr);
    EXPECT_NE(else_if_stmt->then_branch, nullptr);
    EXPECT_NE(else_if_stmt->else_branch, nullptr);
    auto* else_block = dynamic_cast<nota::ast::BlockStmt*>(else_if_stmt->else_branch.get());
    ASSERT_NE(else_block, nullptr);
}

TEST(ParserTest, WhileStmt) {
    std::string source = "while x > 5\n let y = 10\nend\n";
    nota::Lexer lexer(source);
    nota::Parser parser(lexer);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
    auto* while_stmt = dynamic_cast<nota::ast::WhileStmt*>(stmts[0].get());
    ASSERT_NE(while_stmt, nullptr);
    EXPECT_NE(while_stmt->body, nullptr);
}

TEST(ParserTest, DoWhileStmt) {
    std::string source = "do\n let y = 10\nwhile x > 5\n";
    nota::Lexer lexer(source);
    nota::Parser parser(lexer);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
    auto* do_while_stmt = dynamic_cast<nota::ast::DoWhileStmt*>(stmts[0].get());
    ASSERT_NE(do_while_stmt, nullptr);
    EXPECT_NE(do_while_stmt->body, nullptr);
}

TEST(ParserTest, Assignment) {
    std::string source = "x = 10\n";
    nota::Lexer lexer(source);
    nota::Parser parser(lexer);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
    auto* expr_stmt = dynamic_cast<nota::ast::ExpressionStmt*>(stmts[0].get());
    ASSERT_NE(expr_stmt, nullptr);
    auto* assign_expr = dynamic_cast<nota::ast::AssignExpr*>(expr_stmt->expression.get());
    ASSERT_NE(assign_expr, nullptr);
    auto* var_expr = dynamic_cast<nota::ast::VariableExpr*>(assign_expr->name.get());
    ASSERT_NE(var_expr, nullptr);
    EXPECT_EQ(var_expr->name.lexeme, "x");
}

TEST(ParserTest, VariableInExpression) {
    std::string source = "let a = 10\nlet b = a + 5\n";
    nota::Lexer lexer(source);
    nota::Parser parser(lexer);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 2);
    auto* var_decl = dynamic_cast<nota::ast::VarDeclStmt*>(stmts[1].get());
    ASSERT_NE(var_decl, nullptr);
    auto* binary_expr = dynamic_cast<nota::ast::BinaryExpr*>(var_decl->initializer.get());
    ASSERT_NE(binary_expr, nullptr);
    auto* var_expr = dynamic_cast<nota::ast::VariableExpr*>(binary_expr->left.get());
    ASSERT_NE(var_expr, nullptr);
    EXPECT_EQ(var_expr->name.lexeme, "a");
}

TEST(ParserTest, ForEachStmt) {
    std::string source = "for i: 10\n\nend\n";
    nota::Lexer lexer(source);
    nota::Parser parser(lexer);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
}

TEST(ParserTest, PostfixExpr) {
    std::string source = "x++\n";
    nota::Lexer lexer(source);
    nota::Parser parser(lexer);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
}

TEST(ParserTest, ForStmt) {
    std::string source = "for i = 0; i < 10; i++\n\nend\n";
    nota::Lexer lexer(source);
    nota::Parser parser(lexer);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
}

TEST(ParserTest, MatchStmt) {
    std::string source = "match x\n1:\n\nend\n2:\n\nend\nend\n";
    nota::Lexer lexer(source);
    nota::Parser parser(lexer);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
}

TEST(ParserTest, FuncDecl) {
    std::string source = "func add(a: int, b: int): int\n\nend\n";
    nota::Lexer lexer(source);
    nota::Parser parser(lexer);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
}

TEST(ParserTest, CallExpr) {
    std::string source = "add(1, 2)\n";
    nota::Lexer lexer(source);
    nota::Parser parser(lexer);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
}
