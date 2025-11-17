#include "gtest/gtest.h"
#include "Lexer.h"
#include "Parser.h"
#include "AST/AstPrinter.h"
#include <vector>

TEST(LambdaParserTest, SingleExpressionLambda) {
    std::string source = "let add = (a: int, b: int) => a + b\n";
    nota::Lexer lexer(source);
    nota::Parser parser(lexer);
    auto stmts = parser.parse();
    nota::ast::AstPrinter printer;
    std::string result = printer.print(stmts);
    ASSERT_EQ(result, "(var-decl add (lambda (a: int b: int) (+ a b)))");
}

TEST(LambdaParserTest, BlockBodyLambda) {
    std::string source = "let add = (a: int, b: int) =>\n return a + b\nend\n";
    nota::Lexer lexer(source);
    nota::Parser parser(lexer);
    auto stmts = parser.parse();
    nota::ast::AstPrinter printer;
    std::string result = printer.print(stmts);
    ASSERT_EQ(result, "(var-decl add (lambda (a: int b: int) (block (return (+ a b)))))");
}

TEST(LambdaParserTest, NoParameterLambda) {
    std::string source = "let get_num = () => 10\n";
    nota::Lexer lexer(source);
    nota::Parser parser(lexer);
    auto stmts = parser.parse();
    nota::ast::AstPrinter printer;
    std::string result = printer.print(stmts);
    ASSERT_EQ(result, "(var-decl get_num (lambda () 10))");
}
