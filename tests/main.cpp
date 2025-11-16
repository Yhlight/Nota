#include <gtest/gtest.h>
#include "Version.h"

TEST(VersionTest, GetVersion) {
    EXPECT_EQ(nota::getVersion(), "0.0.1");
}
