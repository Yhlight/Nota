#include <gtest/gtest.h>
#include "Compiler.h"

TEST(CompilerTest, BasicAssertions) {
  EXPECT_EQ(add(1, 1), 2);
}
