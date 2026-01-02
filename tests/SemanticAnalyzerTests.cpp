#include <gtest/gtest.h>
#include "SemanticAnalyzer.hpp"
#include "Parser.hpp"
#include "Lexer.hpp"

using namespace Nota;

TEST(SemanticAnalyzerTest, ValidComponent) {
    std::string source = R"(
        App {
            width: 100%;
            height: 60;
            color: #f0f0f0;
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer.ScanTokens());
    auto ast = parser.Parse();
    ASSERT_NE(ast, nullptr);

    SemanticAnalyzer analyzer;
    analyzer.Analyze(*ast);

    EXPECT_TRUE(analyzer.Errors().empty());
}

TEST(SemanticAnalyzerTest, InvalidWidthType) {
    std::string source = R"(
        App {
            width: "100%"; // Invalid type
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer.ScanTokens());
    auto ast = parser.Parse();
    ASSERT_NE(ast, nullptr);

    SemanticAnalyzer analyzer;
    analyzer.Analyze(*ast);

    ASSERT_FALSE(analyzer.Errors().empty());
    EXPECT_NE(analyzer.Errors()[0].find("expects a number or percentage"), std::string::npos);
}

TEST(SemanticAnalyzerTest, InvalidColorType) {
    std::string source = R"(
        App {
            color: 123; // Invalid type
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer.ScanTokens());
    auto ast = parser.Parse();
    ASSERT_NE(ast, nullptr);

    SemanticAnalyzer analyzer;
    analyzer.Analyze(*ast);

    ASSERT_FALSE(analyzer.Errors().empty());
    EXPECT_NE(analyzer.Errors()[0].find("expects a hex color value"), std::string::npos);
}

TEST(SemanticAnalyzerTest, DuplicateComponentId) {
    std::string source = R"(
        App {
            id: "myApp";
            Row {
                id: "myApp"; // Duplicate ID
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer.ScanTokens());
    auto ast = parser.Parse();
    ASSERT_NE(ast, nullptr);

    SemanticAnalyzer analyzer;
    analyzer.Analyze(*ast);

    ASSERT_FALSE(analyzer.Errors().empty());
    EXPECT_NE(analyzer.Errors()[0].find("is already defined"), std::string::npos);
}

TEST(SemanticAnalyzerTest, ValidNestedStructure) {
    std::string source = R"(
        App {
            Row {
                id: "header";
                spacing: 20;
            }
            Col {
                padding: 40;
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer.ScanTokens());
    auto ast = parser.Parse();
    ASSERT_NE(ast, nullptr);

    SemanticAnalyzer analyzer;
    analyzer.Analyze(*ast);

    EXPECT_TRUE(analyzer.Errors().empty());
}
