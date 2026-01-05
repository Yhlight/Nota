#include <gtest/gtest.h>
#include "Lexer.h"
#include "Parser.h"
#include "AST/Stmt.h"
#include "AST/Expr.h"

TEST(ParserModuleTest, ParsePackageStatement) {
    std::string source = "package MyUI.Components;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    auto* package_stmt = dynamic_cast<PackageStmt*>(statements[0].get());
    ASSERT_NE(package_stmt, nullptr);

    // For now, we'll just check that it's a GetExpr.
    // A more robust test would check the structure of the GetExpr.
    auto* name_expr = dynamic_cast<GetExpr*>(package_stmt->name.get());
    ASSERT_NE(name_expr, nullptr);
}

TEST(ParserModuleTest, ParseExportStatement) {
    std::string source = "export { Button, Input };";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    auto* export_stmt = dynamic_cast<ExportStmt*>(statements[0].get());
    ASSERT_NE(export_stmt, nullptr);

    ASSERT_EQ(export_stmt->exports.size(), 2);
    auto* export_1 = dynamic_cast<IdentifierExpr*>(export_stmt->exports[0].get());
    ASSERT_NE(export_1, nullptr);
    ASSERT_EQ(export_1->name.lexeme, "Button");

    auto* export_2 = dynamic_cast<IdentifierExpr*>(export_stmt->exports[1].get());
    ASSERT_NE(export_2, nullptr);
    ASSERT_EQ(export_2->name.lexeme, "Input");
}
