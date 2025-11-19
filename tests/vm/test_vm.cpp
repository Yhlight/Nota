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
