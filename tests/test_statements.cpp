#include "vendor/doctest.h"
#include "../src/lib/Lexer.h"
#include "../src/lib/Parser.h"
#include "../src/lib/Compiler.h"
#include "../src/lib/VM.h"

void testStatement(const std::string& source, const std::string& varName, double expected) {
    nota::Lexer lexer(source);
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    auto statements = parser.parse();

    nota::Compiler compiler;
    nota::Chunk chunk = compiler.compile(statements);

    nota::VM vm;
    vm.interpret(&chunk);

    nota::Value value = vm.globals[varName];
    CHECK(std::any_cast<double>(value) == expected);
}

TEST_CASE("testing variable declarations") {
    testStatement("let a = 1;", "a", 1.0);
    testStatement("let b = 2;", "b", 2.0);
    testStatement("let c = 1 + 2;", "c", 3.0);
}

TEST_CASE("testing if statements") {
    testStatement("let a = 1; if (true) a = 2; end", "a", 2.0);
    testStatement("let a = 1; if (false) a = 2; end", "a", 1.0);
    testStatement("let a = 1; if (true) a = 2; else a = 3; end", "a", 2.0);
    testStatement("let a = 1; if (false) a = 2; else a = 3; end", "a", 3.0);
    testStatement("let a = 1; if (1 < 2) a = 2; else a = 3; end", "a", 2.0);
    testStatement("let a = 1; if (1 > 2) a = 2; else a = 3; end", "a", 3.0);
}

TEST_CASE("testing while loops") {
    testStatement("let a = 0; while (a < 5) a = a + 1; end", "a", 5.0);
}

TEST_CASE("testing for loops") {
    testStatement("let a = 0; for (let i = 0; i < 5; i = i + 1) a = a + 1; end", "a", 5.0);
}
