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

TEST(VMTest, ArithmeticExpression) {
    std::vector<std::pair<std::string, int64_t>> tests = {
        {"1 + 2", 3},
        {"1 - 2", -1},
        {"2 * 3", 6},
        {"4 / 2", 2},
        {"1 + 2 * 3", 7},
        {"(1 + 2) * 3", 9},
        {"(8 - 4) / 2", 2},
        {"2 * (3 + 3) / 2", 6},
    };

    for (const auto& test : tests) {
        VM vm;
        VM::InterpretResult result = vm.Interpret(test.first);
        ASSERT_EQ(result, VM::INTERPRET_OK);
        NotaValue value = vm.Pop();
        ASSERT_TRUE(value.IsInt());
        ASSERT_EQ(value.AsInt(), test.second);
    }
}
