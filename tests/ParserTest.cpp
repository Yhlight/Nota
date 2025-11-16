#include "gtest/gtest.h"
#include "Parser.h"
#include "Lexer.h"
#include "AST/Stmt.h"
#include "AST/Expr.h"

using namespace nota;

TEST(ParserTest, VarDecl) {
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

    EXPECT_EQ(literal->value.lexeme, "10");
}
