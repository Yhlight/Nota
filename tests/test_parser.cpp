#include "vendor/doctest.h"
#include "../src/lib/Lexer.h"
#include "../src/lib/Parser.h"
#include "../src/lib/Expr.h"

TEST_CASE("testing the parser") {
    std::string source = "1 + 2";
    nota::Lexer lexer(source);
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    auto expr = parser.parse();

    CHECK(expr != nullptr);
    auto binaryExpr = dynamic_cast<nota::Binary*>(expr.get());
    CHECK(binaryExpr != nullptr);

    auto left = dynamic_cast<nota::Literal*>(binaryExpr->left.get());
    CHECK(left != nullptr);
    CHECK(std::any_cast<double>(left->value) == 1.0);

    CHECK(binaryExpr->op.type == nota::TokenType::PLUS);

    auto right = dynamic_cast<nota::Literal*>(binaryExpr->right.get());
    CHECK(right != nullptr);
    CHECK(std::any_cast<double>(right->value) == 2.0);
}
