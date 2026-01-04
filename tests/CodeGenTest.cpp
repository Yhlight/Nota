#include <gtest/gtest.h>
#include "Lexer.h"
#include "Parser.h"
#include "Resolver.h"
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
    CodeGenerator generator(resolver.get_custom_types());
    CompilationResult result = generator.generate(statements);

    std::string expected_html = read_file_content("tests/golden/simple.html");
    std::string expected_css = read_file_content("tests/golden/simple.css");

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
    CodeGenerator generator(resolver.get_custom_types());
    CompilationResult result = generator.generate(statements);

    std::string expected_html = read_file_content("tests/golden/custom_component.html");
    std::string expected_css = read_file_content("tests/golden/custom_component.css");

    ASSERT_EQ(result.html, expected_html);
    ASSERT_EQ(result.css, expected_css);
}
