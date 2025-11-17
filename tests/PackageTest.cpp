#include "gtest/gtest.h"
#include "TestHelper.h"
#include "ModuleManager.h"

class PackageTest : public ::testing::Test {
protected:
    void SetUp() override {
        get_module_manager().reset();
    }
};

TEST_F(PackageTest, ImportFromPackage) {
    interpret_file("../../tests/packages/my_package/my_module.nota");
    std::string source = "import my_package::my_module\n";
    ASSERT_NO_THROW(interpret_with_search_path(source, "../../tests/packages"));
}

TEST_F(PackageTest, NamespaceResolutionWithPackage) {
    interpret_file("../../tests/packages/my_package/my_module.nota");
    std::string source = "import my_package::my_module\nlet a = my_module::greet()\n";
    ASSERT_NO_THROW(interpret_with_search_path(source, "../../tests/packages"));
}
