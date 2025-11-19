#include <gtest/gtest.h>
#include "vm/VM.hpp"
#include "core/NotaValue.hpp"

using namespace nota::vm;
using namespace nota::core;

TEST(VMTest, IntegerExpression) {
    VM vm;
    VM::InterpretResult result = vm.Interpret("5");
    ASSERT_EQ(result, VM::INTERPRET_OK);
    NotaValue value = vm.Pop();
    ASSERT_TRUE(value.IsInt());
    ASSERT_EQ(value.AsInt(), 5);
}
