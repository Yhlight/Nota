#include "vendor/doctest.h"
#include "../src/lib/VM.h"

void testFunction(const std::string& source, double expected) {
    nota::VM vm;
    vm.interpret(source);
    CHECK(std::any_cast<double>(vm.lastPopped) == expected);
}

TEST_CASE("testing functions") {
    testFunction("fn test() return 1; end test();", 1.0);
    testFunction("fn add(a, b) return a + b; end add(1, 2);", 3.0);
    testFunction("fn fib(n) if (n < 2) return n; end return fib(n - 1) + fib(n - 2); end fib(10);", 55.0);
}
