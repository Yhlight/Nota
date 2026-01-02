#include "gtest/gtest.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "analyzer/SemanticAnalyzer.h"
#include "codegen/CodeGenerator.h"

TEST(ItemComponentTests, DefineAndUseItem) {
    std::string source = R"(
        Item Box {
            Rect {
                width: 100;
                height: 100;
                color: "red";
            }
        }

        App {
            Box {
                color: "blue";
            }
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
