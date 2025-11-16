#include <gtest/gtest.h>
#include "vm/vm.hpp"

using namespace nota;

TEST(VMTest, WhileLoop) {
    VM vm;
    InterpretResult result = vm.interpret("mut a = 0 while a < 10 end a = a + 1 end");
    EXPECT_EQ(result, InterpretResult::OK);
}

TEST(VMTest, DoWhileLoop) {
    VM vm;
    InterpretResult result = vm.interpret("mut a = 0 do a = a + 1 while a < 10");
    EXPECT_EQ(result, InterpretResult::OK);
}

TEST(VMTest, IfStatement) {
    VM vm;
    InterpretResult result = vm.interpret("let a = 10 if a > 5 end a = 20 end");
    EXPECT_EQ(result, InterpretResult::OK);
}

TEST(VMTest, IfElseStatement) {
    VM vm;
    InterpretResult result = vm.interpret("let a = 10 if a > 15 end a = 20 else a = 30 end");
    EXPECT_EQ(result, InterpretResult::OK);
}

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
