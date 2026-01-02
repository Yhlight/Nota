#include "gtest/gtest.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "analyzer/SemanticAnalyzer.h"

TEST(SemanticAnalyzerTests, ValidInput) {
    std::string source = R"(
        App {
            width: "100%";
            height: 100;
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    RootNode ast = parser.parse();
    ASSERT_TRUE(parser.errors().empty());

    SemanticAnalyzer analyzer;
    EXPECT_TRUE(analyzer.analyze(ast));
    EXPECT_TRUE(analyzer.errors().empty());
}

TEST(SemanticAnalyzerTests, RedefinedProperty) {
    std::string source = R"(
        App {
            width: "100%";
            width: 200;
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    RootNode ast = parser.parse();
    ASSERT_TRUE(parser.errors().empty());

    SemanticAnalyzer analyzer;
    EXPECT_FALSE(analyzer.analyze(ast));
    ASSERT_EQ(analyzer.errors().size(), 1);
    EXPECT_EQ(analyzer.errors()[0], "Property 'width' redefined.");
}

TEST(SemanticAnalyzerTests, InvalidProperty) {
    std::string source = R"(
        App {
            invalid_prop: 100;
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    RootNode ast = parser.parse();
    ASSERT_TRUE(parser.errors().empty());

    SemanticAnalyzer analyzer;
    EXPECT_FALSE(analyzer.analyze(ast));
    ASSERT_EQ(analyzer.errors().size(), 1);
    EXPECT_EQ(analyzer.errors()[0], "Property 'invalid_prop' is not a valid property of 'App'.");
}

TEST(SemanticAnalyzerTests, SpacingInRect) {
    std::string source = R"(
        Rect {
            spacing: 10;
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    RootNode ast = parser.parse();
    ASSERT_TRUE(parser.errors().empty());

    SemanticAnalyzer analyzer;
    EXPECT_FALSE(analyzer.analyze(ast));
    ASSERT_EQ(analyzer.errors().size(), 1);
    EXPECT_EQ(analyzer.errors()[0], "Property 'spacing' is not a valid property of 'Rect'.");
}
