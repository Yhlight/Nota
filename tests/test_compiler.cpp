#include "vendor/doctest.h"
#include "../src/lib/Lexer.h"
#include "../src/lib/Parser.h"
#include "../src/lib/Compiler.h"
#include "../src/lib/VM.h"

void testNumericExpression(const std::string& source, double expected) {
    std::string test_source = "let test_result = " + source;
    nota::VM vm;
    vm.interpret(test_source);

    CHECK(vm.globals.count("test_result") == 1);
    nota::Value value = vm.globals["test_result"];
    CHECK(std::any_cast<double>(value) == expected);
}

void testBooleanExpression(const std::string& source, bool expected) {
    std::string test_source = "let test_result = " + source;
    nota::VM vm;
    vm.interpret(test_source);

    CHECK(vm.globals.count("test_result") == 1);
    nota::Value value = vm.globals["test_result"];
    CHECK(std::any_cast<bool>(value) == expected);
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
