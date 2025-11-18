#include <gtest/gtest.h>
#include "Parser.h"
#include "Tokenizer.h"
#include "PrettyPrinter.h"

TEST(ParserTest, ParsesSimpleExpression) {
    std::string source = "1 + 2 * 3";
    Tokenizer tokenizer(source);
    std::vector<Token> tokens = tokenizer.tokenize();
    Parser parser(tokens);
    std::unique_ptr<Expr> expr = parser.parse();

    PrettyPrinter printer;
    std::string result = printer.print(*expr);

    EXPECT_EQ(result, "(+ 1 (* 2 3))");
}

TEST(ParserTest, ParsesExpressionWithParentheses) {
    std::string source = "(1 + 2) * 3";
    Tokenizer tokenizer(source);
    std::vector<Token> tokens = tokenizer.tokenize();
    Parser parser(tokens);
    std::unique_ptr<Expr> expr = parser.parse();

    PrettyPrinter printer;
    std::string result = printer.print(*expr);

    EXPECT_EQ(result, "(* (+ 1 2) 3)");
}
