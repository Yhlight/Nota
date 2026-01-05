#include <gtest/gtest.h>
#include "Lexer.h"
#include "Parser.h"
#include "AST/Stmt.h"
#include "AST/Expr.h"

TEST(ParserImportTest, ParseSingleImport) {
    std::string source = "import \"./components/Button.nota\";";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    auto* import_stmt = dynamic_cast<ImportStmt*>(statements[0].get());
    ASSERT_NE(import_stmt, nullptr);

    auto* path_expr = dynamic_cast<LiteralExpr*>(import_stmt->path.get());
    ASSERT_NE(path_expr, nullptr);

    ASSERT_EQ(std::any_cast<std::string>(path_expr->value), "./components/Button.nota");
}
