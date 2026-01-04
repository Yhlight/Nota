#include <gtest/gtest.h>
#include "Lexer.h"
#include "Parser.h"
#include "AST/Stmt.h"
#include <vector>
#include <memory>

TEST(ParserTest, ParsesSimpleComponent) {
    std::string source = "App {}";
    nota::Lexer lexer(source);
    std::vector<nota::Token> tokens = lexer.scan_tokens();

    nota::Parser parser(tokens);
    std::vector<std::unique_ptr<nota::ast::Stmt>> ast = parser.parse();

    ASSERT_EQ(ast.size(), 1);

    auto* component_stmt = dynamic_cast<nota::ast::ComponentStmt*>(ast[0].get());
    ASSERT_NE(component_stmt, nullptr);
    EXPECT_EQ(component_stmt->name.lexeme, "App");
    EXPECT_TRUE(component_stmt->body.empty());
}
