#include <gtest/gtest.h>
#include "generator.h"
#include "parser.h"
#include "lexer.h"

TEST(ComponentsTest, ItemDefinitionAndUsage) {
    // Item Box { width: 50 } App { Box { color: red } }
    std::string source = "Item Box { width: 50 } App { Box { color: red } }";
    nota::Lexer lexer(source);
    nota::Parser parser(lexer.tokenize());
    auto program = parser.parse();

    nota::Generator generator;
    std::string output = generator.generate(program);

    // Should contain class definition for Box instance, NOT the template itself
    // The instance should have width: 50px (from template) and background-color: red (from instance)
    // The type in class name should be Box (instance type)

    EXPECT_NE(output.find(".nota-Box-1"), std::string::npos); // First instance
    EXPECT_NE(output.find("width: 50px;"), std::string::npos);
    EXPECT_NE(output.find("background-color: red;"), std::string::npos);
}

TEST(ComponentsTest, NestedChildrenInheritance) {
    // Item Box { Rect { width: 10 } } App { Box { } }
    std::string source = "Item Box { Rect { width: 10 } } App { Box { } }";
    nota::Lexer lexer(source);
    nota::Parser parser(lexer.tokenize());
    auto program = parser.parse();

    nota::Generator generator;
    std::string output = generator.generate(program);

    // The generated Box instance should have a child Rect
    EXPECT_NE(output.find(".nota-Box-1"), std::string::npos);
    EXPECT_NE(output.find(".nota-Rect-2"), std::string::npos); // Child of Box
    EXPECT_NE(output.find("width: 10px;"), std::string::npos);
}
