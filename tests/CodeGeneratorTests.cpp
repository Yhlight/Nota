#include <gtest/gtest.h>
#include "../src/Lexer.hpp"
#include "../src/Parser.hpp"
#include "../src/CodeGenerator.hpp"
#include "../src/AST.hpp"

// Helper function to normalize string for comparison
std::string normalize_string(std::string str) {
    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
    // remove quotes
    str.erase(std::remove(str.begin(), str.end(), '"'), str.end());
    return str;
}

TEST(CodeGeneratorTest, GeneratesCodeForSimpleComponent) {
    std::string source = "Rect { width: 100; color: \"blue\" }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::unique_ptr<Component> root = parser.parse();

    CodeGenerator generator;
    generator.generate(*root);

    std::string expectedHTML = R"(<div class="nota-0"></div>)";
    std::string expectedCSS =
        ".nota-0 {\n"
        "    width: 100px;\n"
        "    background-color: blue;\n"
        "}\n";

    EXPECT_EQ(normalize_string(generator.getHTML()), normalize_string(expectedHTML));
    EXPECT_EQ(normalize_string(generator.getCSS()), normalize_string(expectedCSS));
}

TEST(CodeGeneratorTest, GeneratesCodeForNestedComponents) {
    std::string source = R"(
        Row {
            spacing: 10
            Rect { width: 50 }
            Text { text: "hello" }
        }
    )";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::unique_ptr<Component> root = parser.parse();

    CodeGenerator generator;
    generator.generate(*root);

    std::string expectedHTML = R"(
        <div class="nota-0">
            <div class="nota-1"></div>
            <span class="nota-2">hello</span>
        </div>
    )";
    std::string expectedCSS =
        ".nota-0 {\n"
        "    gap: 10px;\n"
        "}\n"
        ".nota-1 {\n"
        "    width: 50px;\n"
        "}\n"
        ".nota-2 {\n"
        "}\n";

    EXPECT_EQ(normalize_string(generator.getHTML()), normalize_string(expectedHTML));
    EXPECT_EQ(normalize_string(generator.getCSS()), normalize_string(expectedCSS));
}

TEST(CodeGeneratorTest, HandlesTextPropertyAndUnits) {
    std::string source = R"(Text { text: "hello world"; width: "50%" })";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::unique_ptr<Component> root = parser.parse();

    CodeGenerator generator;
    generator.generate(*root);

    std::string expectedHTML = R"(<span class="nota-0">hello world</span>)";
    std::string expectedCSS =
        ".nota-0 {\n"
        "    width: 50%;\n"
        "}\n";

    EXPECT_EQ(normalize_string(generator.getHTML()), normalize_string(expectedHTML));
    EXPECT_EQ(normalize_string(generator.getCSS()), normalize_string(expectedCSS));
}
