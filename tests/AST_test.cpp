#include <gtest/gtest.h>
#include "ast/AST.h"
#include "ast/ASTPrinter.h"
#include "lexer/Token.h"

TEST(ASTPrinterTest, PrintsSimpleBinaryExpression) {
    auto expression = std::make_unique<Binary>(
        std::make_unique<Literal>(123),
        Token{TokenType::PLUS, "+", std::monostate{}, 1},
        std::make_unique<Literal>(456)
    );

    ASTPrinter printer;
    std::string result = printer.print(*expression);
    EXPECT_EQ(result, "(+ 123 456)");
}

TEST(ASTPrinterTest, PrintsNestedExpression) {
    auto expression = std::make_unique<Binary>(
        std::make_unique<Unary>(
            Token{TokenType::MINUS, "-", std::monostate{}, 1},
            std::make_unique<Literal>(123)
        ),
        Token{TokenType::ASTERISK, "*", std::monostate{}, 1},
        std::make_unique<Grouping>(
            std::make_unique<Literal>(45.67)
        )
    );

    ASTPrinter printer;
    std::string result = printer.print(*expression);
    EXPECT_EQ(result, "(* (- 123) (group 45.670000))");
}
