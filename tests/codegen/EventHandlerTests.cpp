#include <gtest/gtest.h>
#include "codegen/CodeGenerator.h"
#include "parser/Parser.h"
#include "lexer/Lexer.h"
#include <string>

TEST(CodeGeneratorTest, EventHandlerTest) {
    std::string source = R"-(
        App {
            Rect {
                width: 100;
                height: 50;
                color: "red";
                onClick: "alert('Hello, World!')";
            }
        }
    )-";

    Lexer lexer(source);
    Parser parser(lexer);
    RootNode ast = parser.parse();

    ASSERT_TRUE(parser.errors().empty());

    CodeGenerator generator;
    std::string html_output = generator.generate(ast);

    // Check for the onclick attribute in the generated HTML
    std::string expected_attribute = "onclick=\"alert('Hello, World!')\"";
    EXPECT_NE(html_output.find(expected_attribute), std::string::npos);
}
