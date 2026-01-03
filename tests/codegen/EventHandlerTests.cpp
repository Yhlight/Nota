#include "codegen/CodeGenerator.h"
#include "parser/Parser.h"
#include "lexer/Lexer.h"
#include <gtest/gtest.h>
#include <string>
#include <regex>

TEST(CodeGenerator, EventHandlerTest) {
    std::string source = R"(
        App {
            id: "myApp";
            onClick: {
                console.log("Hello, World!");
            }
        }
    )";

    Lexer lexer(source);
    Parser parser(lexer);
    RootNode root = parser.parse();

    // The parser should not produce errors for this valid syntax.
    ASSERT_TRUE(parser.errors().empty());

    CodeGenerator generator;
    std::string output = generator.generate(root);

    // The generator should not produce errors.
    ASSERT_TRUE(generator.errors().empty());

    // Check for the presence of the script tag and the event listener.
    ASSERT_TRUE(output.find("<script>") != std::string::npos);
    ASSERT_TRUE(output.find("document.addEventListener('DOMContentLoaded', function() {") != std::string::npos);

    // Check that an element with the correct ID is being looked for.
    ASSERT_TRUE(output.find("document.getElementById('myApp')") != std::string::npos);

    // Check for the correct event listener registration.
    // Updated to expect the function signature with the 'event' parameter.
    ASSERT_TRUE(output.find("addEventListener('click', function(event) {") != std::string::npos);

    // Check that the body of the event handler is correctly placed.
    ASSERT_TRUE(output.find("console.log(\"Hello, World!\");") != std::string::npos);
}
