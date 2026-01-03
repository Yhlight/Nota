#include "analyzer/SemanticAnalyzer.h"
#include "parser/Parser.h"
#include "lexer/Lexer.h"
#include <gtest/gtest.h>

TEST(SemanticAnalyzerTest, ValidEventHandler) {
    std::string source = R"(
        App {
            onClick: {
                // valid handler
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

TEST(SemanticAnalyzerTest, InvalidEventHandlerName) {
    std::string source = R"(
        App {
            onInvalidEvent: {
                // invalid handler
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
    EXPECT_EQ(analyzer.errors()[0].message, "Unknown event 'onInvalidEvent' on component 'App'.");
}
