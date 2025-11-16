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
