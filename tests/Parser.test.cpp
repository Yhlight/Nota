#include <gtest/gtest.h>
#include "Parser.h"
#include "Lexer.h"
#include "AstPrinter.h"

TEST(ParserTest, ParsesVarDeclaration) {
    std::string source = "var a = 10;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);

    AstPrinter printer;
    std::string result = printer.print(*statements[0]);
    EXPECT_EQ(result, "(var a 10)");
}

TEST(ParserTest, ParsesLetDeclaration) {
    std::string source = "let b = 20;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);

    AstPrinter printer;
    std::string result = printer.print(*statements[0]);
    EXPECT_EQ(result, "(var b 20)");
}

TEST(ParserTest, ParsesMutDeclaration) {
    std::string source = "mut c = 30;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);

    AstPrinter printer;
    std::string result = printer.print(*statements[0]);
    EXPECT_EQ(result, "(var c 30)");
}

TEST(ParserTest, ParsesPrintStatement) {
    std::string source = "print 1 + 2;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);

    AstPrinter printer;
    std::string result = printer.print(*statements[0]);
    EXPECT_EQ(result, "(print (+ 1 2))");
}

TEST(ParserTest, ParsesExpressionStatement) {
    std::string source = "1 * 2;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);

    AstPrinter printer;
    std::string result = printer.print(*statements[0]);
    EXPECT_EQ(result, "(; (* 1 2))");
}
