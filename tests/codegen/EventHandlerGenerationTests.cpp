#include "codegen/CodeGenerator.h"
#include "parser/Parser.h"
#include "lexer/Lexer.h"
#include <gtest/gtest.h>
#include <string>
#include <regex>

TEST(CodeGenerator, ComprehensiveEventHandlerTest) {
    std::string source = R"(
        App {
            id: "myApp";
            onClick: { console.log("clicked"); }
            onHover: { this.style.color = 'red'; }
            onKeyPress: { console.log("key pressed"); }
            onChange: { console.log("changed"); }
            onSubmit: { event.preventDefault(); }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    RootNode root = parser.parse();
    CodeGenerator generator;
    std::string result = generator.generate(root);

    // Check for click handler
    ASSERT_TRUE(result.find("addEventListener('click', function(event) {") != std::string::npos);
    ASSERT_TRUE(result.find("console.log(\"clicked\");") != std::string::npos);

    // Check for hover handler
    ASSERT_TRUE(result.find("addEventListener('mouseover', function(event) {") != std::string::npos);
    ASSERT_TRUE(result.find("this.style.color = 'red';") != std::string::npos);

    // Check for keypress handler
    ASSERT_TRUE(result.find("addEventListener('keypress', function(event) {") != std::string::npos);
    ASSERT_TRUE(result.find("console.log(\"key pressed\");") != std::string::npos);

    // Check for change handler
    ASSERT_TRUE(result.find("addEventListener('change', function(event) {") != std::string::npos);
    ASSERT_TRUE(result.find("console.log(\"changed\");") != std::string::npos);

    // Check for submit handler
    ASSERT_TRUE(result.find("addEventListener('submit', function(event) {") != std::string::npos);
    ASSERT_TRUE(result.find("event.preventDefault();") != std::string::npos);
}
