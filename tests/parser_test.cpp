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

TEST(ParserTest, IfStatement) {
    std::string source = "if 1 < 2 let x = 1 end";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    Program program = parser.parse();

    ASSERT_EQ(program.size(), 1);

    auto* if_stmt = dynamic_cast<IfStmt*>(program[0].get());
    ASSERT_NE(if_stmt, nullptr);

    auto* cond_expr = dynamic_cast<BinaryExpr*>(if_stmt->condition.get());
    ASSERT_NE(cond_expr, nullptr);
    ASSERT_EQ(cond_expr->op.type, TokenType::LessThan);

    auto* then_block = dynamic_cast<BlockStmt*>(if_stmt->then_branch.get());
    ASSERT_NE(then_block, nullptr);
    ASSERT_EQ(then_block->statements.size(), 1);

    auto* var_decl = dynamic_cast<VarDeclStmt*>(then_block->statements[0].get());
    ASSERT_NE(var_decl, nullptr);
    ASSERT_EQ(var_decl->name.text, "x");
}

TEST(ParserTest, IfElseStatement) {
    std::string source = "if 1 < 2 let x = 1 else let y = 2 end";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    Program program = parser.parse();

    ASSERT_EQ(program.size(), 1);

    auto* if_stmt = dynamic_cast<IfStmt*>(program[0].get());
    ASSERT_NE(if_stmt, nullptr);

    auto* then_block = dynamic_cast<BlockStmt*>(if_stmt->then_branch.get());
    ASSERT_NE(then_block, nullptr);
    ASSERT_EQ(then_block->statements.size(), 1);

    auto* else_block = dynamic_cast<BlockStmt*>(if_stmt->else_branch.get());
    ASSERT_NE(else_block, nullptr);
    ASSERT_EQ(else_block->statements.size(), 1);
}

TEST(ParserTest, IfElseIfElseStatement) {
    std::string source = "if 1 < 2 let x = 1 else if 2 < 3 let y = 2 else let z = 3 end";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    Program program = parser.parse();

    ASSERT_EQ(program.size(), 1);

    auto* if_stmt = dynamic_cast<IfStmt*>(program[0].get());
    ASSERT_NE(if_stmt, nullptr);

    // Check the 'else if' part, which is a nested IfStmt in the else branch
    auto* else_if_stmt = dynamic_cast<IfStmt*>(if_stmt->else_branch.get());
    ASSERT_NE(else_if_stmt, nullptr);

    // Check the final 'else' part
    auto* final_else_block = dynamic_cast<BlockStmt*>(else_if_stmt->else_branch.get());
    ASSERT_NE(final_else_block, nullptr);
    ASSERT_EQ(final_else_block->statements.size(), 1);
}

TEST(ParserTest, NestedIfStatement) {
    std::string source = "if 1 < 2 if 2 < 3 let x = 1 end let y = 2 end";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    Program program = parser.parse();

    ASSERT_EQ(program.size(), 1);

    auto* outer_if = dynamic_cast<IfStmt*>(program[0].get());
    ASSERT_NE(outer_if, nullptr);

    auto* outer_block = dynamic_cast<BlockStmt*>(outer_if->then_branch.get());
    ASSERT_NE(outer_block, nullptr);
    ASSERT_EQ(outer_block->statements.size(), 2);

    auto* inner_if = dynamic_cast<IfStmt*>(outer_block->statements[0].get());
    ASSERT_NE(inner_if, nullptr);

    auto* inner_block = dynamic_cast<BlockStmt*>(inner_if->then_branch.get());
    ASSERT_NE(inner_block, nullptr);
    ASSERT_EQ(inner_block->statements.size(), 1);
}

TEST(ParserTest, MatchStatement) {
    std::string source = "match x 1: let a = 1 _: let b = 2 end";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    Program program = parser.parse();

    ASSERT_EQ(program.size(), 1);

    auto* match_stmt = dynamic_cast<MatchStmt*>(program[0].get());
    ASSERT_NE(match_stmt, nullptr);
    ASSERT_EQ(match_stmt->cases.size(), 2);

    // Check the first case
    ASSERT_EQ(match_stmt->cases[0].values.size(), 1);
    auto* case1_val = dynamic_cast<NumberExpr*>(match_stmt->cases[0].values[0].get());
    ASSERT_NE(case1_val, nullptr);
    ASSERT_EQ(case1_val->value.text, "1");

    // Check the wildcard case
    ASSERT_EQ(match_stmt->cases[1].values.size(), 1);
    auto* case2_val = dynamic_cast<IdentifierExpr*>(match_stmt->cases[1].values[0].get());
    ASSERT_NE(case2_val, nullptr);
    ASSERT_EQ(case2_val->name.text, "_");
}

TEST(ParserTest, MatchStatementMultiValue) {
    std::string source = "match x 1, 2: let a = 1 end";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    Program program = parser.parse();

    ASSERT_EQ(program.size(), 1);

    auto* match_stmt = dynamic_cast<MatchStmt*>(program[0].get());
    ASSERT_NE(match_stmt, nullptr);
    ASSERT_EQ(match_stmt->cases.size(), 1);

    // Check the first case
    ASSERT_EQ(match_stmt->cases[0].values.size(), 2);
    auto* case1_val1 = dynamic_cast<NumberExpr*>(match_stmt->cases[0].values[0].get());
    ASSERT_NE(case1_val1, nullptr);
    ASSERT_EQ(case1_val1->value.text, "1");

    auto* case1_val2 = dynamic_cast<NumberExpr*>(match_stmt->cases[0].values[1].get());
    ASSERT_NE(case1_val2, nullptr);
    ASSERT_EQ(case1_val2->value.text, "2");
}
