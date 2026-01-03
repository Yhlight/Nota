#include <gtest/gtest.h>
#include "codegen/CodeGenerator.h"
#include "parser/Parser.h"
#include "lexer/Lexer.h"
#include <string>

TEST(CodeGeneratorTest, StateInitializationTest) {
    std::string source = R"-(
        App {
            Rect {
                state message: "Hello, World!";
            }
        }
    )-";

    Lexer lexer(source);
    Parser parser(lexer);
    RootNode ast = parser.parse();

    ASSERT_TRUE(parser.errors().empty());

    CodeGenerator generator;
    std::string html_output = generator.generate(ast);

    // Check for the state initialization script (more robustly)
    EXPECT_NE(html_output.find("NotaRuntime.initState('nota-component-0'"), std::string::npos);
    EXPECT_NE(html_output.find("initialState['message'] = \"Hello, World!\""), std::string::npos);
}

TEST(CodeGeneratorTest, StateUpdateTest) {
    std::string source = R"-(
        App {
            Rect {
                state message: "Hello, World!";
                onClick: "message = 'Goodbye, World!'";
            }
        }
    )-";

    Lexer lexer(source);
    Parser parser(lexer);
    RootNode ast = parser.parse();

    ASSERT_TRUE(parser.errors().empty());

    CodeGenerator generator;
    std::string html_output = generator.generate(ast);

    // Check for the state update script
    std::string expected_script = "onclick=\"NotaRuntime.setState('nota-component-0', 'message', 'Goodbye, World!')\"";
    EXPECT_NE(html_output.find(expected_script), std::string::npos);
}

TEST(CodeGeneratorTest, DataBindingTest) {
    std::string source = R"-(
        App {
            Text {
                state message: "Hello, World!";
                text: message;
            }
        }
    )-";

    Lexer lexer(source);
    Parser parser(lexer);
    RootNode ast = parser.parse();

    ASSERT_TRUE(parser.errors().empty());

    CodeGenerator generator;
    std::string html_output = generator.generate(ast);

    // Check for the data binding script
    std::string expected_script = "NotaRuntime.subscribe('nota-component-0', 'message', (value) => {";
    EXPECT_NE(html_output.find(expected_script), std::string::npos);
}
