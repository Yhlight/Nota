#include <gtest/gtest.h>
#include "AstPrinter.h"
#include "Expr.h"
#include "Token.h"

TEST(AstPrinterTest, PrintsNestedExpression) {
    // Create a representation of: -123 * (45.67)
    auto expression = std::make_unique<Binary>(
        std::make_unique<Unary>(
            Token{TokenType::MINUS, "-", std::monostate{}, 1},
            std::make_unique<Literal>(123.0)
        ),
        Token{TokenType::STAR, "*", std::monostate{}, 1},
        std::make_unique<Grouping>(
            std::make_unique<Literal>(45.67)
        )
    );

    AstPrinter printer;
    std::string result = printer.print(*expression);
    EXPECT_EQ(result, "(* (- 123) (group 45.67))");
}
