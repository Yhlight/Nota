#include "gtest/gtest.h"
#include "Parser.h"
#include "Lexer.h"
#include "AST/Stmt.h"
#include "AST/Expr.h"

using namespace nota;

TEST(ParserTest, VarDeclInt) {
    std::string source = "let x = 10\n";
    Lexer lexer(source);
    Parser parser(lexer);

    std::vector<std::unique_ptr<ast::Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);

    ast::VarDeclStmt* var_decl = dynamic_cast<ast::VarDeclStmt*>(statements[0].get());
    ASSERT_NE(var_decl, nullptr);

    EXPECT_EQ(var_decl->name.lexeme, "x");

    ast::LiteralExpr* literal = dynamic_cast<ast::LiteralExpr*>(var_decl->initializer.get());
    ASSERT_NE(literal, nullptr);

    EXPECT_EQ(literal->value.type, TokenType::Integer);
    EXPECT_EQ(literal->value.lexeme, "10");
}

TEST(ParserTest, VarDeclFloat) {
    std::string source = "let y = 10.5\n";
    Lexer lexer(source);
    Parser parser(lexer);

    std::vector<std::unique_ptr<ast::Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);

    ast::VarDeclStmt* var_decl = dynamic_cast<ast::VarDeclStmt*>(statements[0].get());
    ASSERT_NE(var_decl, nullptr);

    EXPECT_EQ(var_decl->name.lexeme, "y");

    ast::LiteralExpr* literal = dynamic_cast<ast::LiteralExpr*>(var_decl->initializer.get());
    ASSERT_NE(literal, nullptr);

    EXPECT_EQ(literal->value.type, TokenType::Float);
    EXPECT_EQ(literal->value.lexeme, "10.5");
}

TEST(ParserTest, VarDeclWithType) {
    std::string source = "let z: int = 20\n";
    Lexer lexer(source);
    Parser parser(lexer);

    std::vector<std::unique_ptr<ast::Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);

    ast::VarDeclStmt* var_decl = dynamic_cast<ast::VarDeclStmt*>(statements[0].get());
    ASSERT_NE(var_decl, nullptr);

    EXPECT_EQ(var_decl->name.lexeme, "z");
    ASSERT_TRUE(var_decl->type.has_value());
    EXPECT_EQ(var_decl->type->lexeme, "int");

    ast::LiteralExpr* literal = dynamic_cast<ast::LiteralExpr*>(var_decl->initializer.get());
    ASSERT_NE(literal, nullptr);

    EXPECT_EQ(literal->value.type, TokenType::Integer);
    EXPECT_EQ(literal->value.lexeme, "20");
}

TEST(ParserTest, UnaryExpr) {
    std::string source = "let a = -10\n";
    Lexer lexer(source);
    Parser parser(lexer);

    std::vector<std::unique_ptr<ast::Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);

    ast::VarDeclStmt* var_decl = dynamic_cast<ast::VarDeclStmt*>(statements[0].get());
    ASSERT_NE(var_decl, nullptr);

    ast::UnaryExpr* unary = dynamic_cast<ast::UnaryExpr*>(var_decl->initializer.get());
    ASSERT_NE(unary, nullptr);

    EXPECT_EQ(unary->op.type, TokenType::Minus);

    ast::LiteralExpr* literal = dynamic_cast<ast::LiteralExpr*>(unary->right.get());
    ASSERT_NE(literal, nullptr);

    EXPECT_EQ(literal->value.lexeme, "10");
}

TEST(ParserTest, BinaryExpr) {
    std::string source = "let b = 10 + 20 * 30\n";
    Lexer lexer(source);
    Parser parser(lexer);

    std::vector<std::unique_ptr<ast::Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);

    ast::VarDeclStmt* var_decl = dynamic_cast<ast::VarDeclStmt*>(statements[0].get());
    ASSERT_NE(var_decl, nullptr);

    ast::BinaryExpr* binary = dynamic_cast<ast::BinaryExpr*>(var_decl->initializer.get());
    ASSERT_NE(binary, nullptr);

    EXPECT_EQ(binary->op.type, TokenType::Plus);

    ast::LiteralExpr* left_literal = dynamic_cast<ast::LiteralExpr*>(binary->left.get());
    ASSERT_NE(left_literal, nullptr);
    EXPECT_EQ(left_literal->value.lexeme, "10");

    ast::BinaryExpr* right_binary = dynamic_cast<ast::BinaryExpr*>(binary->right.get());
    ASSERT_NE(right_binary, nullptr);
    EXPECT_EQ(right_binary->op.type, TokenType::Star);
}

TEST(ParserTest, LogicalExpr) {
    std::string source = "let c = true && false || true\n";
    Lexer lexer(source);
    Parser parser(lexer);

    std::vector<std::unique_ptr<ast::Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);

    ast::VarDeclStmt* var_decl = dynamic_cast<ast::VarDeclStmt*>(statements[0].get());
    ASSERT_NE(var_decl, nullptr);

    ast::BinaryExpr* or_expr = dynamic_cast<ast::BinaryExpr*>(var_decl->initializer.get());
    ASSERT_NE(or_expr, nullptr);
    EXPECT_EQ(or_expr->op.type, TokenType::PipePipe);

    ast::BinaryExpr* and_expr = dynamic_cast<ast::BinaryExpr*>(or_expr->left.get());
    ASSERT_NE(and_expr, nullptr);
    EXPECT_EQ(and_expr->op.type, TokenType::AmpersandAmpersand);
}

TEST(ParserTest, IfStmt) {
    std::string source = "if (true)\n let x = 1\n end\n";
    Lexer lexer(source);
    Parser parser(lexer);

    std::vector<std::unique_ptr<ast::Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);

    ast::IfStmt* if_stmt = dynamic_cast<ast::IfStmt*>(statements[0].get());
    ASSERT_NE(if_stmt, nullptr);

    ast::LiteralExpr* condition = dynamic_cast<ast::LiteralExpr*>(if_stmt->condition.get());
    ASSERT_NE(condition, nullptr);
    EXPECT_EQ(condition->value.type, TokenType::True);

    ast::BlockStmt* then_branch = dynamic_cast<ast::BlockStmt*>(if_stmt->then_branch.get());
    ASSERT_NE(then_branch, nullptr);
    EXPECT_EQ(then_branch->statements.size(), 1);
}

TEST(ParserTest, IfElseStmt) {
    std::string source = "if (false)\n let x = 1\n else\n let y = 2\n end\n";
    Lexer lexer(source);
    Parser parser(lexer);

    std::vector<std::unique_ptr<ast::Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);

    ast::IfStmt* if_stmt = dynamic_cast<ast::IfStmt*>(statements[0].get());
    ASSERT_NE(if_stmt, nullptr);

    ast::BlockStmt* else_branch = dynamic_cast<ast::BlockStmt*>(if_stmt->else_branch.get());
    ASSERT_NE(else_branch, nullptr);
    EXPECT_EQ(else_branch->statements.size(), 1);
}

TEST(ParserTest, IfElseIfElseStmt) {
    std::string source = "if (false)\n let x = 1\n else if (true)\n let y = 2\n else\n let z = 3\n end\n";
    Lexer lexer(source);
    Parser parser(lexer);

    std::vector<std::unique_ptr<ast::Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);

    ast::IfStmt* if_stmt = dynamic_cast<ast::IfStmt*>(statements[0].get());
    ASSERT_NE(if_stmt, nullptr);

    ast::IfStmt* else_if_stmt = dynamic_cast<ast::IfStmt*>(if_stmt->else_branch.get());
    ASSERT_NE(else_if_stmt, nullptr);

    ast::BlockStmt* else_branch = dynamic_cast<ast::BlockStmt*>(else_if_stmt->else_branch.get());
    ASSERT_NE(else_branch, nullptr);
    EXPECT_EQ(else_branch->statements.size(), 1);
}

TEST(ParserTest, WhileStmt) {
    std::string source = "while (true)\n let x = 1\n end\n";
    Lexer lexer(source);
    Parser parser(lexer);

    std::vector<std::unique_ptr<ast::Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);

    ast::WhileStmt* while_stmt = dynamic_cast<ast::WhileStmt*>(statements[0].get());
    ASSERT_NE(while_stmt, nullptr);

    ast::LiteralExpr* condition = dynamic_cast<ast::LiteralExpr*>(while_stmt->condition.get());
    ASSERT_NE(condition, nullptr);
    EXPECT_EQ(condition->value.type, TokenType::True);

    ast::BlockStmt* body = dynamic_cast<ast::BlockStmt*>(while_stmt->body.get());
    ASSERT_NE(body, nullptr);
    EXPECT_EQ(body->statements.size(), 1);
}

TEST(ParserTest, DoWhileStmt) {
    std::string source = "do\n let x = 1\n while (true)\n";
    Lexer lexer(source);
    Parser parser(lexer);

    std::vector<std::unique_ptr<ast::Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);

    ast::DoWhileStmt* do_while_stmt = dynamic_cast<ast::DoWhileStmt*>(statements[0].get());
    ASSERT_NE(do_while_stmt, nullptr);

    ast::LiteralExpr* condition = dynamic_cast<ast::LiteralExpr*>(do_while_stmt->condition.get());
    ASSERT_NE(condition, nullptr);
    EXPECT_EQ(condition->value.type, TokenType::True);

    ast::BlockStmt* body = dynamic_cast<ast::BlockStmt*>(do_while_stmt->body.get());
    ASSERT_NE(body, nullptr);
    EXPECT_EQ(body->statements.size(), 1);
}

TEST(ParserTest, Assignment) {
    std::string source = "x = 10\n";
    Lexer lexer(source);
    Parser parser(lexer);

    std::vector<std::unique_ptr<ast::Stmt>> statements = parser.parse();
    ASSERT_EQ(statements.size(), 1);

    auto* expr_stmt = dynamic_cast<ast::ExpressionStmt*>(statements[0].get());
    ASSERT_NE(expr_stmt, nullptr);

    auto* assign_expr = dynamic_cast<ast::AssignExpr*>(expr_stmt->expression.get());
    ASSERT_NE(assign_expr, nullptr);

    EXPECT_EQ(assign_expr->name.lexeme, "x");
}

TEST(ParserTest, VariableInExpression) {
    std::string source = "let y = x + 5\n";
    Lexer lexer(source);
    Parser parser(lexer);

    std::vector<std::unique_ptr<ast::Stmt>> statements = parser.parse();
    ASSERT_EQ(statements.size(), 1);

    auto* var_decl = dynamic_cast<ast::VarDeclStmt*>(statements[0].get());
    ASSERT_NE(var_decl, nullptr);

    auto* binary_expr = dynamic_cast<ast::BinaryExpr*>(var_decl->initializer.get());
    ASSERT_NE(binary_expr, nullptr);

    auto* var_expr = dynamic_cast<ast::VariableExpr*>(binary_expr->left.get());
    ASSERT_NE(var_expr, nullptr);
    EXPECT_EQ(var_expr->name.lexeme, "x");
}

TEST(ParserTest, ForEachStmt) {
    std::string source = "for i: array\n let x = 1\n end\n";
    Lexer lexer(source);
    Parser parser(lexer);

    std::vector<std::unique_ptr<ast::Stmt>> statements = parser.parse();
    ASSERT_EQ(statements.size(), 1);

    auto* for_each_stmt = dynamic_cast<ast::ForEachStmt*>(statements[0].get());
    ASSERT_NE(for_each_stmt, nullptr);

    EXPECT_EQ(for_each_stmt->variable.lexeme, "i");

    auto* container = dynamic_cast<ast::VariableExpr*>(for_each_stmt->container.get());
    ASSERT_NE(container, nullptr);
    EXPECT_EQ(container->name.lexeme, "array");

    auto* body = dynamic_cast<ast::BlockStmt*>(for_each_stmt->body.get());
    ASSERT_NE(body, nullptr);
    EXPECT_EQ(body->statements.size(), 1);
}

TEST(ParserTest, PostfixExpr) {
    std::string source = "i++\n";
    Lexer lexer(source);
    Parser parser(lexer);

    std::vector<std::unique_ptr<ast::Stmt>> statements = parser.parse();
    ASSERT_EQ(statements.size(), 1);

    auto* expr_stmt = dynamic_cast<ast::ExpressionStmt*>(statements[0].get());
    ASSERT_NE(expr_stmt, nullptr);

    auto* postfix_expr = dynamic_cast<ast::PostfixExpr*>(expr_stmt->expression.get());
    ASSERT_NE(postfix_expr, nullptr);

    EXPECT_EQ(postfix_expr->op.type, TokenType::PlusPlus);
}

TEST(ParserTest, ForStmt) {
    std::string source = "for let i = 0; i < 10; i++\n let x = 1\n end\n";
    Lexer lexer(source);
    Parser parser(lexer);

    std::vector<std::unique_ptr<ast::Stmt>> statements = parser.parse();
    ASSERT_EQ(statements.size(), 1);

    auto* for_stmt = dynamic_cast<ast::ForStmt*>(statements[0].get());
    ASSERT_NE(for_stmt, nullptr);

    ASSERT_NE(for_stmt->initializer, nullptr);
    ASSERT_NE(for_stmt->condition, nullptr);
    ASSERT_NE(for_stmt->increment, nullptr);
    ASSERT_NE(for_stmt->body, nullptr);
}
