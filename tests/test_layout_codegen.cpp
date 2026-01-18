#include <gtest/gtest.h>
#include "Lexer.h"
#include "Parser.h"
#include "CodeGen.h"

// ... existing tests ...

TEST(LayoutTest, SpacingMapping) {
    std::string input = "Row { spacing: 20 }";
    Lexer lexer(input);
    Parser parser(lexer.tokenize());
    auto root = parser.parse();

    CodeGen codegen;
    std::string html = codegen.generateHTML(*root);

    EXPECT_NE(html.find("gap: 20px"), std::string::npos);
}

TEST(LayoutTest, PaddingMapping) {
    std::string input = "Col { padding: 10 }";
    Lexer lexer(input);
    Parser parser(lexer.tokenize());
    auto root = parser.parse();

    CodeGen codegen;
    std::string html = codegen.generateHTML(*root);

    EXPECT_NE(html.find("padding: 10px"), std::string::npos);
}

TEST(LayoutTest, BorderMapping) {
    std::string input = "Rect { border: \"1px solid black\" }";
    Lexer lexer(input);
    Parser parser(lexer.tokenize());
    auto root = parser.parse();

    CodeGen codegen;
    std::string html = codegen.generateHTML(*root);

    EXPECT_NE(html.find("border: 1px solid black"), std::string::npos);
}

TEST(LayoutTest, AbsolutePositioning) {
    std::string input = "Rect { x: 50; y: 100 }";
    Lexer lexer(input);
    Parser parser(lexer.tokenize());
    auto root = parser.parse();

    CodeGen codegen;
    std::string html = codegen.generateHTML(*root);

    EXPECT_NE(html.find("left: 50px"), std::string::npos);
    EXPECT_NE(html.find("top: 100px"), std::string::npos);
    EXPECT_NE(html.find("position: absolute"), std::string::npos);
}

TEST(LayoutTest, PositionCenter) {
    std::string input = "Rect { position: center }";
    Lexer lexer(input);
    Parser parser(lexer.tokenize());
    auto root = parser.parse();

    CodeGen codegen;
    std::string html = codegen.generateHTML(*root);

    EXPECT_NE(html.find("left: 50%"), std::string::npos);
    EXPECT_NE(html.find("top: 50%"), std::string::npos);
    EXPECT_NE(html.find("transform: translate(-50%, -50%)"), std::string::npos);
    EXPECT_NE(html.find("position: absolute"), std::string::npos);
}

TEST(LayoutTest, PositionLeftTop) {
    std::string input = "Rect { position: left top }";
    Lexer lexer(input);
    Parser parser(lexer.tokenize());
    auto root = parser.parse();

    CodeGen codegen;
    std::string html = codegen.generateHTML(*root);

    EXPECT_NE(html.find("left: 0"), std::string::npos);
    EXPECT_NE(html.find("top: 0"), std::string::npos);
    EXPECT_NE(html.find("position: absolute"), std::string::npos);
}
