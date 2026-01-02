#include <gtest/gtest.h>
#include "../src/Lexer.hpp"
#include "../src/Parser.hpp"
#include "../src/SemanticAnalyzer.hpp"
#include "../src/AST.hpp"

TEST(SemanticAnalyzerTest, AcceptsValidSpacingProperty) {
    std::string source = "Row { spacing: 10 }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::unique_ptr<Component> root = parser.parse();

    SemanticAnalyzer analyzer;
    analyzer.analyze(*root);

    EXPECT_TRUE(analyzer.getErrors().empty());
}

TEST(SemanticAnalyzerTest, RejectsInvalidSpacingProperty) {
    std::string source = "App { spacing: 10 }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::unique_ptr<Component> root = parser.parse();

    SemanticAnalyzer analyzer;
    analyzer.analyze(*root);

    ASSERT_EQ(analyzer.getErrors().size(), 1);
    EXPECT_EQ(analyzer.getErrors()[0], "Undefined property 'spacing' for component 'App'.");
}
