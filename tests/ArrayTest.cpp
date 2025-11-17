#include "gtest/gtest.h"
#include "TestHelper.h"
#include <vector>

TEST(ArrayTest, ArrayLiteral) {
    std::string source = "let a = [1, 2, 3]\n";
    ASSERT_NO_THROW(interpret(source));
}

TEST(ArrayTest, SubscriptGet) {
    std::string source = "let a = [1, 2, 3]\nlet b = a[1]\n";
    ASSERT_NO_THROW(interpret(source));
}

TEST(ArrayTest, SubscriptSet) {
    std::string source = "let a = [1, 2, 3]\na[1] = 5\n";
    ASSERT_NO_THROW(interpret(source));
}

TEST(ArrayTest, OutOfBounds) {
    std::string source = "let a = [1, 2, 3]\nlet b = a[3]\n";
    ASSERT_THROW(interpret(source), std::runtime_error);
}
