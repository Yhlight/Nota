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
