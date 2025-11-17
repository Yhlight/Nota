#include "gtest/gtest.h"
#include "TestHelper.h"

TEST(ModuleTest, ImportModule) {
    std::string source = "import \"math.nota\"\n";
    ASSERT_NO_THROW(interpret(source));
}

TEST(ModuleTest, ImportWithAlias) {
    std::string source = "import \"math.nota\" as m\n";
    ASSERT_NO_THROW(interpret(source));
}

TEST(ModuleTest, NamespaceResolution) {
    std::string source = "import \"math.nota\"\nlet a = math::add(1, 2)\n";
    ASSERT_NO_THROW(interpret(source));
}

TEST(ModuleTest, NamespaceResolutionWithAlias) {
    std::string source = "import \"math.nota\" as m\nlet a = m::add(1, 2)\n";
    ASSERT_NO_THROW(interpret(source));
}
