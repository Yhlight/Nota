#include <gtest/gtest.h>
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "ast/ASTPrinter.h"
#include <string>

std::string parseAndPrint(const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(std::move(tokens));
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();
    if (!statements.empty() && statements[0]) {
        ASTPrinter printer;
        return printer.print(*statements[0]);
    }
    return "Parsing failed";
}

TEST(ParserTest, ParsesSimpleAddition) {
    std::string result = parseAndPrint("1 + 2");
    EXPECT_EQ(result, "(; (+ 1 2))");
}

TEST(ParserTest, HandlesOperatorPrecedence) {
    std::string result = parseAndPrint("1 + 2 * 3");
    EXPECT_EQ(result, "(; (+ 1 (* 2 3)))");
}

TEST(ParserTest, HandlesParentheses) {
    std::string result = parseAndPrint("(1 + 2) * 3");
    EXPECT_EQ(result, "(; (* (group (+ 1 2)) 3))");
}

TEST(ParserTest, HandlesUnaryOperators) {
    std::string result = parseAndPrint("-1 * !true");
    EXPECT_EQ(result, "(; (* (- 1) (! true)))");
}

TEST(ParserTest, HandlesComparisonOperators) {
    std::string result = parseAndPrint("1 < 2 == 3 > 4");
    EXPECT_EQ(result, "(; (== (< 1 2) (> 3 4)))");
}

TEST(ParserTest, ReportsErrorOnMissingClosingParen) {
    std::string result = parseAndPrint("(1 + 2");
    EXPECT_EQ(result, "Parsing failed");
}

TEST(ParserTest, ParsesVariableDeclaration) {
    std::string result = parseAndPrint("let x = 10");
    EXPECT_EQ(result, "(var x 10)");
}

TEST(ParserTest, ParsesVariableDeclarationWithoutInitializer) {
    std::string result = parseAndPrint("let x");
    EXPECT_EQ(result, "(var x)");
}

TEST(ParserTest, ParsesIfStatement) {
    std::string result = parseAndPrint("if true 1");
    EXPECT_EQ(result, "(if true (; 1))");
}

TEST(ParserTest, ParsesIfElseStatement) {
    std::string result = parseAndPrint("if true 1 else 2");
    EXPECT_EQ(result, "(if-else true (; 1) (; 2))");
}
