#include "vendor/doctest.h"
#include "../src/lib/Lexer.h"
#include "../src/lib/Parser.h"
#include "../src/lib/Compiler.h"
#include "../src/lib/VM.h"

void testNumericExpression(const std::string& source, double expected) {
    nota::Lexer lexer(source);
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    auto statements = parser.parse();

    nota::Compiler compiler;
    nota::Chunk chunk = compiler.compile(statements);

    nota::VM vm;
    vm.interpret(&chunk);

    CHECK(std::any_cast<double>(vm.stack.back()) == expected);
}

void testBooleanExpression(const std::string& source, bool expected) {
    nota::Lexer lexer(source);
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    auto statements = parser.parse();

    nota::Compiler compiler;
    nota::Chunk chunk = compiler.compile(statements);

    nota::VM vm;
    vm.interpret(&chunk);

    CHECK(std::any_cast<bool>(vm.stack.back()) == expected);
}

TEST_CASE("testing the compiler with arithmetic") {
    testNumericExpression("1 + 2;", 3.0);
    testNumericExpression("1 - 2;", -1.0);
    testNumericExpression("2 * 3;", 6.0);
    testNumericExpression("6 / 2;", 3.0);
}

TEST_CASE("testing the compiler with comparisons") {
    testBooleanExpression("1 < 2;", true);
    testBooleanExpression("2 < 1;", false);
    testBooleanExpression("2 > 1;", true);
    testBooleanExpression("1 > 2;", false);
    testBooleanExpression("1 == 1;", true);
    testBooleanExpression("1 == 2;", false);
    testBooleanExpression("1 != 2;", true);
    testBooleanExpression("1 != 1;", false);
    testBooleanExpression("1 <= 1;", true);
    testBooleanExpression("1 <= 2;", true);
    testBooleanExpression("2 <= 1;", false);
    testBooleanExpression("2 >= 1;", true);
    testBooleanExpression("1 >= 1;", true);
    testBooleanExpression("1 >= 2;", false);
}
