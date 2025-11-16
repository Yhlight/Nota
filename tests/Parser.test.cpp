#include <gtest/gtest.h>
#include "Parser.h"
#include "Lexer.h"
#include "AstPrinter.h"

TEST(ParserTest, ParsesSimpleExpression) {
    std::string source = "-123 * (45.67)";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::unique_ptr<Expr> expression = parser.parse();

    ASSERT_NE(expression, nullptr);

    AstPrinter printer;
    std::string result = printer.print(*expression);
    EXPECT_EQ(result, "(* (- 123) (group 45.67))");
}
