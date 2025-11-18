#include <gtest/gtest.h>
#include "Lexer.h"
#include "Parser.h"
#include "ASTPrinter.h"
#include <string>

std::string parseAndPrint(const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(std::move(tokens));
    std::unique_ptr<Expr> expr = parser.parse();
    if (expr) {
        ASTPrinter printer;
        return printer.print(*expr);
    }
    return "Parsing failed";
}

TEST(ParserTest, ParsesSimpleAddition) {
    std::string result = parseAndPrint("1 + 2");
    EXPECT_EQ(result, "(+ 1 2)");
}

TEST(ParserTest, HandlesOperatorPrecedence) {
    std::string result = parseAndPrint("1 + 2 * 3");
    EXPECT_EQ(result, "(+ 1 (* 2 3))");
}

TEST(ParserTest, HandlesParentheses) {
    std::string result = parseAndPrint("(1 + 2) * 3");
    EXPECT_EQ(result, "(* (group (+ 1 2)) 3)");
}

TEST(ParserTest, HandlesUnaryOperators) {
    std::string result = parseAndPrint("-1 * !true");
    EXPECT_EQ(result, "(* (- 1) (! true))");
}

TEST(ParserTest, HandlesComparisonOperators) {
    std::string result = parseAndPrint("1 < 2 == 3 > 4");
    EXPECT_EQ(result, "(== (< 1 2) (> 3 4))");
}

TEST(ParserTest, ReportsErrorOnMissingClosingParen) {
    std::string result = parseAndPrint("(1 + 2");
    EXPECT_EQ(result, "Parsing failed");
}
