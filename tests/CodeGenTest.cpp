#include <gtest/gtest.h>
#include "Lexer.h"
#include "Parser.h"
#include "Resolver.h"
#include "Evaluator.h"
#include "CodeGenerator.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>

std::string read_file_content(const std::string& path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

TEST(CodeGenTest, SimpleGoldenFile) {
    std::string source = read_file_content("tests/golden/simple.nota");

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();
    Resolver resolver;
    resolver.resolve(statements);
    Evaluator evaluator;
    evaluator.evaluate(statements);
    CodeGenerator generator(resolver.get_custom_types(), evaluator.get_results());
    CompilationResult result = generator.generate(statements);

    std::string expected_html = read_file_content("tests/golden/simple.html");
    std::string expected_css = read_file_content("tests/golden/simple.css");

    ASSERT_EQ(result.html, expected_html);
    ASSERT_EQ(result.css, expected_css);
}

TEST(CodeGenTest, ExpressionEvaluationGoldenFile) {
    std::string source = read_file_content("tests/golden/expression_evaluation.nota");

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();
    Resolver resolver;
    resolver.resolve(statements);
    Evaluator evaluator;
    evaluator.evaluate(statements);
    CodeGenerator generator(resolver.get_custom_types(), evaluator.get_results());
    CompilationResult result = generator.generate(statements);

    std::string expected_html = read_file_content("tests/golden/expression_evaluation.html");
    std::string expected_css = read_file_content("tests/golden/expression_evaluation.css");

    ASSERT_EQ(result.html, expected_html);
    ASSERT_EQ(result.css, expected_css);
}

TEST(CodeGenTest, PositioningGoldenFile) {
    std::string source = read_file_content("tests/golden/positioning.nota");

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();
    Resolver resolver;
    resolver.resolve(statements);
    Evaluator evaluator;
    evaluator.evaluate(statements);
    CodeGenerator generator(resolver.get_custom_types(), evaluator.get_results());
    CompilationResult result = generator.generate(statements);

    std::string expected_html = read_file_content("tests/golden/positioning.html");
    std::string expected_css = read_file_content("tests/golden/positioning.css");

    ASSERT_EQ(result.html, expected_html);
    ASSERT_EQ(result.css, expected_css);
}

TEST(CodeGenTest, MorePropertiesGoldenFile) {
    std::string source = read_file_content("tests/golden/more_properties.nota");

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();
    Resolver resolver;
    resolver.resolve(statements);
    Evaluator evaluator;
    evaluator.evaluate(statements);
    CodeGenerator generator(resolver.get_custom_types(), evaluator.get_results());
    CompilationResult result = generator.generate(statements);

    std::string expected_html = read_file_content("tests/golden/more_properties.html");
    std::string expected_css = read_file_content("tests/golden/more_properties.css");

    ASSERT_EQ(result.html, expected_html);
    ASSERT_EQ(result.css, expected_css);
}

TEST(CodeGenTest, PropertyOverrideGoldenFile) {
    std::string source = read_file_content("tests/golden/property_override.nota");

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();
    Resolver resolver;
    resolver.resolve(statements);
    Evaluator evaluator;
    evaluator.evaluate(statements);
    CodeGenerator generator(resolver.get_custom_types(), evaluator.get_results());
    CompilationResult result = generator.generate(statements);

    std::string expected_html = read_file_content("tests/golden/property_override.html");
    std::string expected_css = read_file_content("tests/golden/property_override.css");

    ASSERT_EQ(result.html, expected_html);
    ASSERT_EQ(result.css, expected_css);
}

TEST(CodeGenTest, CustomComponentGoldenFile) {
    std::string source = read_file_content("tests/golden/custom_component.nota");

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();
    Resolver resolver;
    resolver.resolve(statements);
    Evaluator evaluator;
    evaluator.evaluate(statements);
    CodeGenerator generator(resolver.get_custom_types(), evaluator.get_results());
    CompilationResult result = generator.generate(statements);

    std::string expected_html = read_file_content("tests/golden/custom_component.html");
    std::string expected_css = read_file_content("tests/golden/custom_component.css");

    ASSERT_EQ(result.html, expected_html);
    ASSERT_EQ(result.css, expected_css);
}
