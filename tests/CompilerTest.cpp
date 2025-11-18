#include <gtest/gtest.h>
#include "Compiler.h"
#include "VM.h"
#include "Lexer.h"
#include "Parser.h"
#include "Value.h"
#include <variant>
#include <vector>

// Test helper to compile and run an expression, returning the result from the VM stack
static nota::Value run_expr(const std::string &source) {
    // The parser expects a newline at the end of a statement.
    nota::Lexer lexer(source + "\n");
    nota::Parser parser(lexer);
    auto stmts = parser.parse();

    nota::Compiler compiler;
    auto chunk = compiler.compile(stmts);

    // For expression tests, we want to inspect the value left on the stack,
    // so we remove the final Pop and Return opcodes.
    chunk.code.pop_back(); // Pop Return
    chunk.code.pop_back(); // Pop Pop

    nota::VM vm;
    vm.interpret(chunk);

    return vm.pop();
}

class CompilerTest : public ::testing::Test {
protected:
    nota::VM vm;

    void run_stmt(const std::string &source) {
        nota::Lexer lexer(source + "\n");
        nota::Parser parser(lexer);
        auto stmts = parser.parse();

        nota::Compiler compiler;
        auto chunk = compiler.compile(stmts);
        vm.interpret(chunk);
    }
};

TEST(Compiler, TestAddition) {
    nota::Value result = run_expr("1 + 2");
    EXPECT_EQ(std::get<long long>(result), 3);
}

TEST(Compiler, TestTrue) {
    nota::Value result = run_expr("true");
    EXPECT_TRUE(std::get<bool>(result));
}

TEST(Compiler, TestFalse) {
    nota::Value result = run_expr("false");
    EXPECT_FALSE(std::get<bool>(result));
}

TEST(Compiler, TestNot) {
    nota::Value result = run_expr("!true");
    EXPECT_FALSE(std::get<bool>(result));
}

TEST(Compiler, TestEqual) {
    nota::Value result = run_expr("1 == 1");
    EXPECT_TRUE(std::get<bool>(result));
}

TEST(Compiler, TestNotEqual) {
    nota::Value result = run_expr("1 != 2");
    EXPECT_TRUE(std::get<bool>(result));
}

TEST(Compiler, TestGreater) {
    nota::Value result = run_expr("2 > 1");
    EXPECT_TRUE(std::get<bool>(result));
}

TEST(Compiler, TestLess) {
    nota::Value result = run_expr("1 < 2");
    EXPECT_TRUE(std::get<bool>(result));
}

TEST(Compiler, TestGreaterEqual) {
    nota::Value result = run_expr("2 >= 1");
    EXPECT_TRUE(std::get<bool>(result));
}

TEST(Compiler, TestLessEqual) {
    nota::Value result = run_expr("1 <= 2");
    EXPECT_TRUE(std::get<bool>(result));
}

TEST(Compiler, TestSubtraction) {
    nota::Value result = run_expr("2 - 1");
    EXPECT_EQ(std::get<long long>(result), 1);
}

TEST(Compiler, TestMultiplication) {
    nota::Value result = run_expr("2 * 3");
    EXPECT_EQ(std::get<long long>(result), 6);
}

TEST(Compiler, TestDivision) {
    nota::Value result = run_expr("6 / 3");
    EXPECT_EQ(std::get<long long>(result), 2);
}

TEST(Compiler, TestChainedArithmetic) {
    nota::Value result = run_expr("1 + 2 * 3 - 4 / 2");
    EXPECT_EQ(std::get<long long>(result), 5);
}

TEST_F(CompilerTest, TestGlobalVariable) {
    run_stmt("let a = 10");
    nota::Value result = vm.get_global("a");
    EXPECT_EQ(std::get<long long>(result), 10);
}

TEST_F(CompilerTest, TestGlobalVariableAssignment) {
    run_stmt("mut a = 10");
    run_stmt("a = 20");
    nota::Value result = vm.get_global("a");
    EXPECT_EQ(std::get<long long>(result), 20);
}

TEST_F(CompilerTest, TestGlobalVariableInExpression) {
    run_stmt("let a = 10");
    run_stmt("let b = 20");
    run_stmt("let c = a + b");
    nota::Value result = vm.get_global("c");
    EXPECT_EQ(std::get<long long>(result), 30);
}

TEST_F(CompilerTest, TestUndefinedVariable) {
    EXPECT_THROW(run_stmt("a = 10"), std::runtime_error);
    EXPECT_THROW(run_stmt("let b = a"), std::runtime_error);
}

TEST_F(CompilerTest, TestString) {
    run_stmt("let a = \"hello\"");
    nota::Value result = vm.get_global("a");
    EXPECT_EQ(std::get<std::string>(result), "hello");
}

TEST_F(CompilerTest, TestStringConcatenation) {
    run_stmt("let a = \"hello\" + \" world\"");
    nota::Value result = vm.get_global("a");
    EXPECT_EQ(std::get<std::string>(result), "hello world");
}

TEST_F(CompilerTest, TestStringEquality) {
    nota::Value result = run_expr("\"hello\" == \"hello\"");
    EXPECT_TRUE(std::get<bool>(result));
}

TEST_F(CompilerTest, TestStringInequality) {
    nota::Value result = run_expr("\"hello\" != \"world\"");
    EXPECT_TRUE(std::get<bool>(result));
}

TEST_F(CompilerTest, TestIfStatement) {
    run_stmt("mut a = 10\nif true\na = 20\nend");
    nota::Value result = vm.get_global("a");
    EXPECT_EQ(std::get<long long>(result), 20);
}

TEST_F(CompilerTest, TestIfStatementFalse) {
    run_stmt("mut a = 10\nif false\na = 20\nend");
    nota::Value result = vm.get_global("a");
    EXPECT_EQ(std::get<long long>(result), 10);
}

TEST_F(CompilerTest, TestIfElseStatement) {
    run_stmt("mut a = 10\nif true\na = 20\nelse\na = 30\nend");
    nota::Value result = vm.get_global("a");
    EXPECT_EQ(std::get<long long>(result), 20);
}

TEST_F(CompilerTest, TestIfElseStatementFalse) {
    run_stmt("mut a = 10\nif false\na = 20\nelse\na = 30\nend");
    nota::Value result = vm.get_global("a");
    EXPECT_EQ(std::get<long long>(result), 30);
}

TEST_F(CompilerTest, TestElseIfStatement) {
    run_stmt("mut a = 10\nif false\na = 20\nelse if true\na = 40\nelse\na = 30\nend");
    nota::Value result = vm.get_global("a");
    EXPECT_EQ(std::get<long long>(result), 40);
}
