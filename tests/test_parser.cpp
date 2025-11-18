#include <gtest/gtest.h>
#include "Parser.h"
#include "Tokenizer.h"
#include "PrettyPrinter.h"

TEST(ParserTest, ParsesSimpleExpression) {
    std::string source = "1 + 2 * 3\n";
    Tokenizer tokenizer(source);
    std::vector<Token> tokens = tokenizer.tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    ExprStmt* stmt = dynamic_cast<ExprStmt*>(statements[0].get());
    ASSERT_NE(stmt, nullptr);

    PrettyPrinter printer;
    std::string result = printer.print(*stmt->expression);

    EXPECT_EQ(result, "(+ 1 (* 2 3))");
}

TEST(ParserTest, ParsesExpressionWithParentheses) {
    std::string source = "(1 + 2) * 3\n";
    Tokenizer tokenizer(source);
    std::vector<Token> tokens = tokenizer.tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    ExprStmt* stmt = dynamic_cast<ExprStmt*>(statements[0].get());
    ASSERT_NE(stmt, nullptr);

    PrettyPrinter printer;
    std::string result = printer.print(*stmt->expression);

    EXPECT_EQ(result, "(* (+ 1 2) 3)");
}
