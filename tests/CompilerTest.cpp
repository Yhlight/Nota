#include <gtest/gtest.h>
#include "Compiler.h"
#include "VM.h"
#include "Lexer.h"
#include "Parser.h"
#include "Value.h"
#include <variant>
#include <vector>

static nota::Value run(const std::string &source) {
    // The parser expects a newline at the end of a statement.
    nota::Lexer lexer(source + "\n");
    nota::Parser parser(lexer);
    auto stmts = parser.parse();

    nota::Compiler compiler;
    auto chunk = compiler.compile(stmts);

    nota::VM vm;
    vm.interpret(chunk);

    return vm.pop();
}

TEST(Compiler, TestAddition) {
    nota::Value result = run("1 + 2");
    EXPECT_EQ(std::get<long long>(result), 3);
}

TEST(Compiler, TestTrue) {
    nota::Value result = run("true");
    EXPECT_TRUE(std::get<bool>(result));
}

TEST(Compiler, TestFalse) {
    nota::Value result = run("false");
    EXPECT_FALSE(std::get<bool>(result));
}

TEST(Compiler, TestNot) {
    nota::Value result = run("!true");
    EXPECT_FALSE(std::get<bool>(result));
}

TEST(Compiler, TestEqual) {
    nota::Value result = run("1 == 1");
    EXPECT_TRUE(std::get<bool>(result));
}

TEST(Compiler, TestNotEqual) {
    nota::Value result = run("1 != 2");
    EXPECT_TRUE(std::get<bool>(result));
}

TEST(Compiler, TestGreater) {
    nota::Value result = run("2 > 1");
    EXPECT_TRUE(std::get<bool>(result));
}

TEST(Compiler, TestLess) {
    nota::Value result = run("1 < 2");
    EXPECT_TRUE(std::get<bool>(result));
}

TEST(Compiler, TestGreaterEqual) {
    nota::Value result = run("2 >= 1");
    EXPECT_TRUE(std::get<bool>(result));
}

TEST(Compiler, TestLessEqual) {
    nota::Value result = run("1 <= 2");
    EXPECT_TRUE(std::get<bool>(result));
}
