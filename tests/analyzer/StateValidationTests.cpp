#include "analyzer/SemanticAnalyzer.h"
#include "parser/Parser.h"
#include "lexer/Lexer.h"
#include <gtest/gtest.h>

TEST(SemanticAnalyzerTest, ValidStateDeclarationAndUsage) {
    std::string source = R"(
        App {
            state myState: "hello";
            Text {
                text: myState;
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    RootNode root = parser.parse();
    SemanticAnalyzer analyzer;
    bool result = analyzer.analyze(root);
    ASSERT_TRUE(result);
}

TEST(SemanticAnalyzerTest, DuplicateStateDeclaration) {
    std::string source = R"(
        App {
            state myState: "hello";
            state myState: "world";
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    RootNode root = parser.parse();
    SemanticAnalyzer analyzer;
    bool result = analyzer.analyze(root);
    ASSERT_FALSE(result);
    ASSERT_EQ(analyzer.errors().size(), 1);
    EXPECT_EQ(analyzer.errors()[0].message, "State variable 'myState' is already defined.");
}

TEST(SemanticAnalyzerTest, UndefinedStateVariable) {
    std::string source = R"(
        App {
            Text {
                text: myState;
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    RootNode root = parser.parse();
    SemanticAnalyzer analyzer;
    bool result = analyzer.analyze(root);
    ASSERT_FALSE(result);
    ASSERT_EQ(analyzer.errors().size(), 1);
    EXPECT_EQ(analyzer.errors()[0].message, "Undefined state variable 'myState'.");
}
