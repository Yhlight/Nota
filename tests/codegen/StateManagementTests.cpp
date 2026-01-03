#include "codegen/CodeGenerator.h"
#include "parser/Parser.h"
#include "lexer/Lexer.h"
#include <gtest/gtest.h>

TEST(CodeGeneratorTest, StateManagement) {
    std::string source = R"(
        App {
            id: "app";
            state message: "Hello";

            Text {
                id: "textLabel";
                text: message;
            }

            Rect {
                id: "button";
                onClick: {
                    NotaRuntime.setState("app", { message: "World" });
                }
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    RootNode root = parser.parse();
    CodeGenerator generator;
    std::string result = generator.generate(root);

    // Check for NotaRuntime definition
    ASSERT_TRUE(result.find("const NotaRuntime = {") != std::string::npos);

    // Check for state registration
    ASSERT_TRUE(result.find("NotaRuntime.register('app', {") != std::string::npos);
    ASSERT_TRUE(result.find("message: \"Hello\",") != std::string::npos);

    // Check for data binding
    ASSERT_TRUE(result.find("NotaRuntime.bind('textLabel', 'message', (value) => {") != std::string::npos);
    ASSERT_TRUE(result.find("document.getElementById('textLabel').innerText = value;") != std::string::npos);

    // Check for setState call in event handler
    ASSERT_TRUE(result.find("NotaRuntime.setState(\"app\", { message: \"World\" });") != std::string::npos);
}
