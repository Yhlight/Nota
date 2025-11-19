#include "vm/VM.hpp"
#include <gtest/gtest.h>

TEST(VMTest, IntegerExpression) {
    nota::vm::VM vm;
    auto result = vm.Interpret("123");
    EXPECT_EQ(result, nota::vm::InterpretResult::OK);
}
