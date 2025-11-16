#include "gtest/gtest.h"
#include "nota.h"

TEST(NotaTest, BasicAssertion) {
    EXPECT_EQ(1, 1);
}

TEST(NotaTest, PrintHello) {
    // This is a bit of a hacky test, but it's just to ensure that the
    // library is linked correctly.
    testing::internal::CaptureStdout();
    print_hello();
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "Hello from the Nota library!\n");
}
