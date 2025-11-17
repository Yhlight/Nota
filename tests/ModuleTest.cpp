#include "gtest/gtest.h"
#include "TestHelper.h"

TEST(ModuleTest, ImportModule) {
    std::string source = "import \"math\"\n";
    ASSERT_NO_THROW(interpret_with_search_path(source, "../../tests"));
}

TEST(ModuleTest, ImportWithAlias) {
    std::string source = "import \"math\" as m\n";
    ASSERT_NO_THROW(interpret_with_search_path(source, "../../tests"));
}

TEST(ModuleTest, NamespaceResolution) {
    std::string source = "import \"math\"\nlet a = math::add(1, 2)\n";
    ASSERT_NO_THROW(interpret_with_search_path(source, "../../tests"));
}

TEST(ModuleTest, NamespaceResolutionWithAlias) {
    std::string source = "import \"math\" as m\nlet a = m::add(1, 2)\n";
    ASSERT_NO_THROW(interpret_with_search_path(source, "../../tests"));
}
