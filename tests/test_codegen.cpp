#include <gtest/gtest.h>
#include "Lexer.h"
#include "Parser.h"
#include "CodeGen.h"

TEST(CodeGenTest, BasicHTML) {
    std::string input = "App { width: 100% }";
    Lexer lexer(input);
    Parser parser(lexer.tokenize());
    auto root = parser.parse();

    CodeGen codegen;
    std::string html = codegen.generateHTML(*root);

    // Expected: <body class="nota-app" style="width: 100%; "></body>
    EXPECT_NE(html.find("<body"), std::string::npos);
    EXPECT_NE(html.find("class=\"nota-app\""), std::string::npos);
    EXPECT_NE(html.find("width: 100%"), std::string::npos);
}

TEST(CodeGenTest, NestedHTML) {
    std::string input = "App { Rect { color: red } }";
    Lexer lexer(input);
    Parser parser(lexer.tokenize());
    auto root = parser.parse();

    CodeGen codegen;
    std::string html = codegen.generateHTML(*root);

    EXPECT_NE(html.find("<div class=\"nota-rect\""), std::string::npos);
    EXPECT_NE(html.find("background-color: red"), std::string::npos);
}

TEST(CodeGenTest, TextComponent) {
    std::string input = "Text { text: \"Hello\" }";
    Lexer lexer(input);
    Parser parser(lexer.tokenize());
    auto root = parser.parse();

    CodeGen codegen;
    std::string html = codegen.generateHTML(*root);

    EXPECT_NE(html.find("<span class=\"nota-text\""), std::string::npos);
    EXPECT_NE(html.find(">Hello</span>"), std::string::npos);
}

TEST(CodeGenTest, FloatValues) {
    std::string input = "Rect { width: 100.5; height: 50.5 }";
    Lexer lexer(input);
    Parser parser(lexer.tokenize());
    auto root = parser.parse();

    CodeGen codegen;
    std::string html = codegen.generateHTML(*root);

    EXPECT_NE(html.find("width: 100.5px"), std::string::npos);
    EXPECT_NE(html.find("height: 50.5px"), std::string::npos);
}
