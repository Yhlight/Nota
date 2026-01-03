#include <gtest/gtest.h>
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "analyzer/SemanticAnalyzer.h"
#include "codegen/CodeGenerator.h"

TEST(CodeGenerator, EventHandlerTest) {
    std::string source = R"(
        App {
            Rect {
                id: "myRect"
                width: 100
                height: 100
                color: "red"

                onClick: {
                    console.log("Clicked!");
                }

                onHover: {
                    this.style.color = "blue";
                }
            }
        }
    )";

    Lexer lexer(source);
    Parser parser(lexer);
    RootNode ast = parser.parse();
    ASSERT_TRUE(parser.errors().empty());

    SemanticAnalyzer analyzer;
    bool success = analyzer.analyze(ast);
    ASSERT_TRUE(success);
    ASSERT_TRUE(analyzer.errors().empty());

    CodeGenerator generator;
    std::string output = generator.generate(ast);

    // Check for the id attribute in the HTML
    ASSERT_NE(output.find(R"(id="myRect")"), std::string::npos);

    // Check for the generated JavaScript
    ASSERT_NE(output.find(R"(const comp0 = document.getElementById('myRect');)"), std::string::npos);
    ASSERT_NE(output.find(R"(comp0.addEventListener('click', function() {)"), std::string::npos);
    ASSERT_NE(output.find(R"(console.log("Clicked!");)"), std::string::npos);
    ASSERT_NE(output.find(R"(comp0.addEventListener('mouseover', function() {)"), std::string::npos);
    ASSERT_NE(output.find(R"(this.style.color = "blue";)"), std::string::npos);
}
