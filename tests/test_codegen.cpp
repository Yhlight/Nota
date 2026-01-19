#include <gtest/gtest.h>
#include "Lexer.h"
#include "Parser.h"
#include "CodeGen.h"

TEST(CodeGenTest, ExpressionEvaluation) {
    std::string input = "Rect { width: 10 + 20 }";
    Lexer lexer(input);
    Parser parser(lexer.tokenize());
    auto nodes = parser.parseAll();

    CodeGen codegen;
    std::string html = codegen.generateHTML(*nodes[0]);

    EXPECT_NE(html.find("width: 30px"), std::string::npos);
}

TEST(CodeGenTest, ExpressionArithmeticOrder) {
    std::string input = "Rect { width: 10 + 20 * 5 }";
    Lexer lexer(input);
    Parser parser(lexer.tokenize());
    auto nodes = parser.parseAll();

    CodeGen codegen;
    std::string html = codegen.generateHTML(*nodes[0]);

    EXPECT_NE(html.find("width: 110px"), std::string::npos);
}

TEST(CodeGenTest, BasicHTML) {
    std::string input = "App { width: 100% }";
    Lexer lexer(input);
    Parser parser(lexer.tokenize());
    auto nodes = parser.parseAll();

    CodeGen codegen;
    std::string html = codegen.generateHTML(*nodes[0]);

    EXPECT_NE(html.find("width: 100%"), std::string::npos);
}

TEST(CodeGenTest, NestedHTML) {
    std::string input = "App { Rect { color: red } }";
    Lexer lexer(input);
    Parser parser(lexer.tokenize());
    auto nodes = parser.parseAll();

    CodeGen codegen;
    std::string html = codegen.generateHTML(*nodes[0]);

    EXPECT_NE(html.find("<div class=\"nota-rect\""), std::string::npos);
    EXPECT_NE(html.find("background-color: red"), std::string::npos);
}

TEST(CodeGenTest, TextComponent) {
    std::string input = "Text { text: \"Hello\" }";
    Lexer lexer(input);
    Parser parser(lexer.tokenize());
    auto nodes = parser.parseAll();

    CodeGen codegen;
    std::string html = codegen.generateHTML(*nodes[0]);

    EXPECT_NE(html.find("<span class=\"nota-text\""), std::string::npos);
    EXPECT_NE(html.find(">Hello</span>"), std::string::npos);
}

TEST(CodeGenTest, FloatValues) {
    std::string input = "Rect { width: 100.5; height: 50.5 }";
    Lexer lexer(input);
    Parser parser(lexer.tokenize());
    auto nodes = parser.parseAll();

    CodeGen codegen;
    std::string html = codegen.generateHTML(*nodes[0]);

    EXPECT_NE(html.find("width: 100.5px"), std::string::npos);
    EXPECT_NE(html.find("height: 50.5px"), std::string::npos);
}

TEST(CodeGenTest, ConditionalRendering) {
    std::string input =
        "App { \n"
        "  if (true) { \n"
        "    Rect { color: red } \n"
        "  } else { \n"
        "    Rect { color: blue } \n"
        "  } \n"
        "}";
    Lexer lexer(input);
    Parser parser(lexer.tokenize());
    auto nodes = parser.parseAll();

    CodeGen codegen;
    std::string html = codegen.generateHTML(*nodes[0]);

    EXPECT_NE(html.find("background-color: red"), std::string::npos);
    EXPECT_EQ(html.find("background-color: blue"), std::string::npos);
}

TEST(CodeGenTest, ConditionalRenderingFalse) {
    std::string input =
        "App { \n"
        "  if (false) { \n"
        "    Rect { color: red } \n"
        "  } else { \n"
        "    Rect { color: blue } \n"
        "  } \n"
        "}";
    Lexer lexer(input);
    Parser parser(lexer.tokenize());
    auto nodes = parser.parseAll();

    CodeGen codegen;
    std::string html = codegen.generateHTML(*nodes[0]);

    EXPECT_EQ(html.find("background-color: red"), std::string::npos);
    EXPECT_NE(html.find("background-color: blue"), std::string::npos);
}

TEST(CodeGenTest, CalcGeneration) {
    std::string input = "Rect { width: 100% - 20px }";
    Lexer lexer(input);
    Parser parser(lexer.tokenize());
    auto nodes = parser.parseAll();

    CodeGen codegen;
    std::string html = codegen.generateHTML(*nodes[0]);

    // Expect calc(100% - 20px)
    EXPECT_NE(html.find("width: calc(100% - 20px)"), std::string::npos);
}

TEST(CodeGenTest, EventHandling) {
    std::string input = "Button { onClick: \"alert('Hi')\"; onHover: \"console.log('Hover')\" }";
    Lexer lexer(input);
    Parser parser(lexer.tokenize());
    auto nodes = parser.parseAll();

    CodeGen codegen;
    std::string html = codegen.generateHTML(*nodes[0]);

    // Check attributes
    EXPECT_NE(html.find("onclick=\"alert('Hi')\""), std::string::npos);
    EXPECT_NE(html.find("onmouseenter=\"console.log('Hover')\""), std::string::npos);

    // Check that these are NOT in style
    size_t stylePos = html.find("style=\"");
    if (stylePos != std::string::npos) {
        size_t styleEnd = html.find("\"", stylePos + 7);
        std::string styleContent = html.substr(stylePos, styleEnd - stylePos);
        EXPECT_EQ(styleContent.find("onClick"), std::string::npos);
        EXPECT_EQ(styleContent.find("onHover"), std::string::npos);
    }
}
