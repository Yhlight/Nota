#include "gtest/gtest.h"
#include "TestHelper.h"
#include <vector>

TEST(ClassTest, ClassDeclaration) {
    std::string source = "class Person\nend\n";
    ASSERT_NO_THROW(interpret(source));
}

TEST(ClassTest, InstanceCreation) {
    std::string source = "class Person\nend\nlet p = Person{}\n";
    ASSERT_NO_THROW(interpret(source));
}

TEST(ClassTest, FieldAccess) {
    std::string source = "class Person\nend\nlet p = Person{}\np.name = \"John\"\nlet name = p.name\n";
    ASSERT_NO_THROW(interpret(source));
}

TEST(ClassTest, MethodCall) {
    std::string source = "class Person\nfunc say_hello()\nreturn \"hello\"\nend\nend\nlet p = Person{}\nlet greeting = p.say_hello()\n";
    ASSERT_NO_THROW(interpret(source));
}

TEST(ClassTest, ThisExpression) {
    std::string source = "class Person\nfunc get_this()\nreturn this\nend\nend\nlet p = Person{}\nlet p2 = p.get_this()\n";
    ASSERT_NO_THROW(interpret(source));
}

TEST(ClassTest, Constructor) {
    std::string source = "class Person\nfunc new(name: string)\nthis.name = name\nend\nend\nlet p = Person{\"John\"}\n";
    ASSERT_NO_THROW(interpret(source));
}
