#include <gtest/gtest.h>
#include "Lexer.h"
#include "Parser.h"
#include "Resolver.h"

TEST(ModuleTest, HandlesBasicPackage) {
    std::string source = "package MyModule;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();
    ASSERT_EQ(statements.size(), 1);
    auto* package_stmt = dynamic_cast<PackageStmt*>(statements[0].get());
    ASSERT_NE(package_stmt, nullptr);
    EXPECT_EQ(package_stmt->name.lexeme, "MyModule");
}
