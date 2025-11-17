#include "gtest/gtest.h"
#include "TestHelper.h"
#include <vector>

TEST(StaticArrayTest, StaticArrayDeclaration) {
    std::string source = "let a: int[10]\n";
    ASSERT_NO_THROW(interpret(source));
}

TEST(StaticArrayTest, StaticArrayInitialization) {
    std::string source = "let a: int[3] = [1, 2, 3]\n";
    ASSERT_NO_THROW(interpret(source));
}

TEST(StaticArrayTest, StaticArrayInitializationTooLarge) {
    std::string source = "let a: int[2] = [1, 2, 3]\n";
    ASSERT_THROW(interpret(source), std::runtime_error);
}
