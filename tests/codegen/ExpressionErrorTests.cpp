#include <gtest/gtest.h>
#include "codegen/CodeGenerator.h"
#include "parser/Parser.h"
#include "lexer/Lexer.h"

TEST(ExpressionErrorTests, OutOfBoundsIndex) {
    std::string source = R"(
        App {
            Row {
                id: r1
            }
            this.children[1].width = 100
        }
    )";

    Lexer lexer(source);
    Parser parser(lexer);
    RootNode ast = parser.parse();
    ASSERT_TRUE(parser.errors().empty());

    CodeGenerator generator;
    generator.generate(ast);

    const auto& errors = generator.errors();
    ASSERT_EQ(errors.size(), 1);
    EXPECT_TRUE(errors[0].message.find("out of bounds") != std::string::npos);
}

TEST(ExpressionErrorTests, NonExistentProperty) {
    std::string source = R"(
        App {
            Rect {} // Add a child to access
            this.children[0].non_existent = "error"
        }
    )";

    Lexer lexer(source);
    Parser parser(lexer);
    RootNode ast = parser.parse();
    ASSERT_TRUE(parser.errors().empty());

    CodeGenerator generator;
    generator.generate(ast);

    const auto& errors = generator.errors();
    ASSERT_EQ(errors.size(), 1);
    EXPECT_TRUE(errors[0].message.find("not found") != std::string::npos);
}
