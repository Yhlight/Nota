#include "vm/VM.hpp"
#include <gtest/gtest.h>

TEST(VMTest, IntegerExpression) {
    nota::vm::VM vm;
    auto result = vm.Interpret("123");
    EXPECT_EQ(result, nota::vm::InterpretResult::OK);
    EXPECT_EQ(vm.GetLastPopped().AsInt(), 123);
}

TEST(VMTest, ArithmeticExpression) {
    nota::vm::VM vm;
    auto result = vm.Interpret("1 + 2 * 3 - 4 / 2");
    EXPECT_EQ(result, nota::vm::InterpretResult::OK);
    EXPECT_EQ(vm.GetLastPopped().AsInt(), 5);
}

TEST(VMTest, GlobalVariable) {
    nota::vm::VM vm;
    auto result = vm.Interpret("let a = 10\na");
    EXPECT_EQ(result, nota::vm::InterpretResult::OK);
    EXPECT_EQ(vm.GetLastPopped().AsInt(), 10);
}

TEST(VMTest, Assignment) {
    nota::vm::VM vm;
    auto result = vm.Interpret("mut a = 10\na = 20\na");
    EXPECT_EQ(result, nota::vm::InterpretResult::OK);
    EXPECT_EQ(vm.GetLastPopped().AsInt(), 20);
}

TEST(VMTest, ProgramFlow) {
    nota::vm::VM vm;
    auto result = vm.Interpret("mut a = 1\na = 5\na + 2");
    EXPECT_EQ(result, nota::vm::InterpretResult::OK);
    EXPECT_EQ(vm.GetLastPopped().AsInt(), 7);
}
