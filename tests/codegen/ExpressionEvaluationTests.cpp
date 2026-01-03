#include "gtest/gtest.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "analyzer/SemanticAnalyzer.h"
#include "codegen/CodeGenerator.h"
#include <iostream>

TEST(ExpressionEvaluationTests, AssignmentToChildProperty) {
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
            this.children[0].children[0].color = "blue";
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

TEST(ExpressionEvaluationTests, ComplexAssignment) {
    std::string source = R"(
        App {
            Col {
                Rect {
                    id: rect1;
                    width: 100;
                }
                Rect {
                    id: rect2;
                    width: 50;
                }
            }
            this.children[0].children[1].width = this.children[0].children[0].width;
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

    EXPECT_NE(output.find("width: 100px;"), std::string::npos);
    EXPECT_EQ(output.find("width: 50px;"), std::string::npos);
}

TEST(ExpressionEvaluationTests, ArithmeticExpressionInProperty) {
    std::string source = R"(
        App {
            Rect {
                width: 100 + 50;
                height: 20 * 3;
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

    EXPECT_NE(output.find("width: 150px;"), std::string::npos);
    EXPECT_NE(output.find("height: 60px;"), std::string::npos);
}

TEST(ExpressionEvaluationTests, ParentPropertyAccessInExpression) {
    std::string source = R"(
        App {
            width: 200;
            Rect {
                width: parent.width / 2;
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

    EXPECT_NE(output.find("width: 100px;"), std::string::npos);
}
