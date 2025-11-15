#include <gtest/gtest.h>
#include "vm/vm.hpp"

using namespace nota;

TEST(VMTest, GlobalVariable) {
    VM vm;
    InterpretResult result = vm.interpret("let a = 10");
    EXPECT_EQ(result, InterpretResult::OK);
    result = vm.interpret("let b = a + 20");
    EXPECT_EQ(result, InterpretResult::OK);
}

TEST(VMTest, UndefinedVariable) {
    VM vm;
    InterpretResult result = vm.interpret("let a = b");
    EXPECT_EQ(result, InterpretResult::RUNTIME_ERROR);
}
