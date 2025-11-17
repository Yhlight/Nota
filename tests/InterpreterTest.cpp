#include "gtest/gtest.h"
#include "TestHelper.h"
#include <vector>

TEST(InterpreterTest, VarDeclAndAssignment) {
    std::string source = "let a = 10\nlet b = a + 5\n";
    ASSERT_NO_THROW(interpret(source));
}

TEST(InterpreterTest, BlockScope) {
    std::string source = "let a = 10\nif a == 10\n let b = 20\nend\n";
    ASSERT_NO_THROW(interpret(source));
}

TEST(InterpreterTest, WhileLoop) {
    std::string source = "mut a = 0\nwhile a < 10\na = a + 1\nend\n";
    ASSERT_NO_THROW(interpret(source));
}

TEST(InterpreterTest, FunctionCall) {
    std::string source = "func add(a: int, b: int): int\n return a + b\nend\nlet c = add(1, 2)\n";
    ASSERT_NO_THROW(interpret(source));
}

TEST(InterpreterTest, Recursion) {
    std::string source = "func fib(n: int): int\n if n <= 1\n return n\nend\n return fib(n - 1) + fib(n - 2)\nend\nlet c = fib(10)\n";
    ASSERT_NO_THROW(interpret(source));
}

TEST(InterpreterTest, LexicalScope) {
    std::string source = "let a = 10\nfunc outer(): function\n let a = 20\n func inner(): int\n return a\nend\n return inner\nend\nlet inner = outer()\nlet c = inner()\n";
    ASSERT_NO_THROW(interpret(source));
}

TEST(InterpreterTest, NativeFunction) {
    std::string source = "let t = clock()\n";
    ASSERT_NO_THROW(interpret(source));
}

TEST(InterpreterTest, ForLoop) {
    std::string source = "mut a = 0\nfor let i = 0; i < 10; i++\na = a + i\nend\n";
    ASSERT_NO_THROW(interpret(source));
}

TEST(InterpreterTest, ForEachLoop) {
    std::string source = "let a = [1, 2, 3]\nmut sum = 0\nfor i: a\nsum = sum + i\nend\n";
    ASSERT_NO_THROW(interpret(source));
}

TEST(InterpreterTest, MatchStatement) {
    std::string source = "let a = 2\nmut b = 0\nmatch a\n1:\nb = 1\nend\n2:\nb = 2\nend\n_:\nb = 3\nend\nend\n";
    ASSERT_NO_THROW(interpret(source));
}

TEST(InterpreterTest, LambdaExpression) {
    std::string source = "let add = (a: int, b: int) => a + b\nlet c = add(1, 2)\n";
    ASSERT_NO_THROW(interpret(source));
}

TEST(InterpreterTest, PostfixExpression) {
    std::string source = "mut a = 10\na++\n";
    ASSERT_NO_THROW(interpret(source));
}
