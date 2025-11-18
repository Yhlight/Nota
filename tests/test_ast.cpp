#include <gtest/gtest.h>
#include "PrettyPrinter.h"
#include "Expr.h"

TEST(ASTTest, PrettyPrintsSimpleAST) {
    auto ast = std::make_unique<BinaryExpr>(
        std::make_unique<UnaryExpr>(
            Token{TokenType::MINUS, "-", {}},
            std::make_unique<LiteralExpr>(Token{TokenType::INTEGER, "123", {}})
        ),
        Token{TokenType::STAR, "*", {}},
        std::make_unique<LiteralExpr>(Token{TokenType::INTEGER, "456", {}})
    );

    PrettyPrinter printer;
    std::string result = printer.print(*ast);

    EXPECT_EQ(result, "(* (- 123) 456)");
}
