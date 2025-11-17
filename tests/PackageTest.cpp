#include "gtest/gtest.h"
#include "TestHelper.h"

TEST(PackageTest, PackageDeclaration) {
    std::string source = "package my_package\n";
    ASSERT_NO_THROW(interpret(source));
}

TEST(PackageTest, ImportFromPackage) {
    interpret_file("../../tests/packages/my_package/my_module.nota");
    std::string source = "import my_package::my_module\n";
    ASSERT_NO_THROW(interpret(source));
}

TEST(PackageTest, NamespaceResolutionWithPackage) {
    interpret_file("../../tests/packages/my_package/my_module.nota");
    std::string source = "import my_package::my_module\nlet a = my_module::greet()\n";
    ASSERT_NO_THROW(interpret(source));
}
