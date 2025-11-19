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

TEST(VMTest, LetStatement) {
    VM vm;
    VM::InterpretResult result = vm.Interpret("let a = 10\n a");
    ASSERT_EQ(result, VM::INTERPRET_OK);
    NotaValue value = vm.Pop();
    ASSERT_TRUE(value.IsInt());
    ASSERT_EQ(value.AsInt(), 10);
}

TEST(VMTest, LetStatementWithExpression) {
    VM vm;
    VM::InterpretResult result = vm.Interpret("let a = 5 + 5\n let b = a * 2\n b");
    ASSERT_EQ(result, VM::INTERPRET_OK);
    NotaValue value = vm.Pop();
    ASSERT_TRUE(value.IsInt());
    ASSERT_EQ(value.AsInt(), 20);
}

TEST(VMTest, FloatExpression) {
    VM vm;
    VM::InterpretResult result = vm.Interpret("3.14");
    ASSERT_EQ(result, VM::INTERPRET_OK);
    NotaValue value = vm.Pop();
    ASSERT_TRUE(value.IsFloat());
    ASSERT_EQ(value.AsFloat(), 3.14);
}

TEST(VMTest, BooleanExpression) {
    std::vector<std::pair<std::string, bool>> tests = {
        {"true", true},
        {"false", false},
    };

    for (const auto& test : tests) {
        VM vm;
        VM::InterpretResult result = vm.Interpret(test.first);
        ASSERT_EQ(result, VM::INTERPRET_OK);
        NotaValue value = vm.Pop();
        ASSERT_TRUE(value.IsBool());
        ASSERT_EQ(value.AsBool(), test.second);
    }
}

TEST(VMTest, ComparisonExpression) {
    std::vector<std::pair<std::string, bool>> tests = {
        {"1 < 2", true},
        {"1 > 2", false},
        {"1 < 1", false},
        {"1 > 1", false},
        {"1 <= 2", true},
        {"1 >= 2", false},
        {"1 <= 1", true},
        {"1 >= 1", true},
        {"1 == 1", true},
        {"1 != 1", false},
        {"1 == 2", false},
        {"1 != 2", true},
    };

    for (const auto& test : tests) {
        VM vm;
        VM::InterpretResult result = vm.Interpret(test.first);
        ASSERT_EQ(result, VM::INTERPRET_OK);
        NotaValue value = vm.Pop();
        ASSERT_TRUE(value.IsBool());
        ASSERT_EQ(value.AsBool(), test.second);
    }
}

TEST(VMTest, StringInterning) {
    VM vm;
    VM::InterpretResult result = vm.Interpret("let a = 10\n let b = 10\n");
    ASSERT_EQ(result, VM::INTERPRET_OK);
    ASSERT_EQ(vm.interned_strings_.size(), 2);
}

TEST(VMTest, LetStatementImmutable) {
    VM vm;
    VM::InterpretResult result = vm.Interpret("let a = 10\n a = 20");
    ASSERT_EQ(result, VM::INTERPRET_RUNTIME_ERROR);
}

TEST(VMTest, MutStatement) {
    VM vm;
    VM::InterpretResult result = vm.Interpret("mut a = 10\n a = 20\n a");
    ASSERT_EQ(result, VM::INTERPRET_OK);
    NotaValue value = vm.Pop();
    ASSERT_TRUE(value.IsInt());
    ASSERT_EQ(value.AsInt(), 20);
}
