#include "gtest/gtest.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "analyzer/SemanticAnalyzer.h"
#include "codegen/CodeGenerator.h"

TEST(RootExpressionTests, AssignmentFromRoot) {
    std::string source = R"(
        App {
            id: myApp;
            Col {
                id: myCol;
                Rect {
                    id: myRect;
                    color: "red";
                }
            }
            root.children[0].children[0].color = "blue";
        }
    )";

    Lexer lexer(source);
    Parser parser(lexer);
    RootNode ast = parser.parse();
    ASSERT_TRUE(parser.errors().empty());

    SemanticAnalyzer analyzer;
    ASSERT_TRUE(analyzer.analyze(ast));

    CodeGenerator generator;
    std::string output = generator.generate(ast);

    EXPECT_NE(output.find("background-color: blue;"), std::string::npos);
    EXPECT_EQ(output.find("background-color: red;"), std::string::npos);
}
